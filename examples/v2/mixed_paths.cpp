/** A controller always receives readings. Optional diagnostic receivers can come and go beside it. */
#include "sub0pub/wiring.hpp"
#include "sub0pub/wiring/broker_port.hpp"

struct TemperatureReading { int celsius; };

struct CoolingController
{
    unsigned readingsReceived = 0;
    bool fanRunning = false;

    void receive(const TemperatureReading& reading) noexcept
    {
        fanRunning = reading.celsius >= 21;
        ++readingsReceived;
    }
};

using DiagnosticPort = sub0::DynamicPort<TemperatureReading, 1>;

struct DiagnosticProbe final : DiagnosticPort::Receiver
{
    unsigned readingsReceived = 0;
    void receive(const TemperatureReading&) noexcept override { ++readingsReceived; }
};

bool attachAndRemoveAProbe()
{
    CoolingController controller;
    DiagnosticPort diagnostics;
    auto wiring = sub0::wire(controller, diagnostics);

    wiring.publish(TemperatureReading{20}); // The controller works even with no diagnostic receiver.
    if (controller.readingsReceived != 1)
        return false;

    DiagnosticProbe probe;
    DiagnosticProbe waitingProbe;
    if (!diagnostics.tryAdd(&probe) || diagnostics.tryAdd(&waitingProbe))
        return false;

    wiring.publish(TemperatureReading{21});
    if (controller.readingsReceived != 2 || probe.readingsReceived != 1)
        return false;

    // DynamicPort borrows unique, non-null receivers. Remove before destruction, outside delivery.
    // Adding, removing and publishing must not run concurrently.
    diagnostics.remove(&probe);
    wiring.publish(TemperatureReading{22});

    return controller.readingsReceived == 3 && controller.fanRunning && probe.readingsReceived == 1;
}

struct SessionTemperatureReading
{
    int celsius;
    using sub0_config = sub0::config<sub0::Scoped, sub0::Snapshot>;
};

struct SessionCoolingController
{
    unsigned readingsReceived = 0;
    bool fanRunning = false;

    void receive(const SessionTemperatureReading& reading) noexcept
    {
        fanRunning = reading.celsius >= 21;
        ++readingsReceived;
    }
};

struct OneShotProbe final : sub0::Subscribe<SessionTemperatureReading>
{
    using Subscribe::Subscribe;
    unsigned readingsReceived = 0;

    void receive(const SessionTemperatureReading&) noexcept override
    {
        ++readingsReceived;
        disconnect(); // BrokerPort with Snapshot permits removal during a callback.
    }
};

bool letProbesDisconnectThemselves()
{
    sub0::Domain<SessionTemperatureReading> diagnosticSession;
    SessionCoolingController controller;
    sub0::BrokerPort<SessionTemperatureReading> diagnostics(diagnosticSession);
    auto wiring = sub0::wire(controller, diagnostics);

    OneShotProbe firstProbe(diagnosticSession);
    OneShotProbe secondProbe(diagnosticSession);
    wiring.publish(SessionTemperatureReading{20});
    if (firstProbe.readingsReceived != 1 || secondProbe.readingsReceived != 1)
        return false;

    wiring.publish(SessionTemperatureReading{21}); // Both probes have disconnected; the controller remains wired.
    if (controller.readingsReceived != 2 || !controller.fanRunning)
        return false;
    diagnosticSession.close();
    wiring.publish(SessionTemperatureReading{22}); // Closing the diagnostic session leaves the fixed path intact.

    return controller.readingsReceived == 3 && controller.fanRunning &&
        firstProbe.readingsReceived == 1 && secondProbe.readingsReceived == 1;
}

int main()
{
    if (!attachAndRemoveAProbe()) return 1;
    if (!letProbesDisconnectThemselves()) return 2;
    return 0;
}

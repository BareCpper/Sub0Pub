/** A thermometer sends readings to known receivers. Choose how much of the wiring the publisher needs to know. */
#include "sub0pub/wiring.hpp"

struct TemperatureReading { int celsius; };
struct SensorFault { unsigned code; };

struct TemperatureDisplay
{
    int lastCelsius = 0;
    unsigned readingsReceived = 0;

    void receive(const TemperatureReading& reading) noexcept
    {
        lastCelsius = reading.celsius;
        ++readingsReceived;
    }
};

struct FaultLog
{
    unsigned faultsReceived = 0;
    void receive(const SensorFault&) noexcept { ++faultsReceived; }
};

struct TemperatureRangeCheck
{
    bool receive(const TemperatureReading& reading) noexcept { return reading.celsius >= -40 && reading.celsius <= 125; }
};

template<class Output>
struct TemperatureSensor final : sub0::Publisher<TemperatureSensor<Output>, Output>
{
    using sub0::Publisher<TemperatureSensor<Output>, Output>::Publisher;
    void measure(int celsius) noexcept { this->publish(TemperatureReading{celsius}); }
};

bool wireLocalReceivers()
{
    TemperatureDisplay display;
    FaultLog faultLog;
    auto wiring = sub0::wire(display, faultLog);
    TemperatureSensor<decltype(wiring)> thermometer(wiring);
    static_assert(sub0::handles_v<TemperatureDisplay, TemperatureReading>, "TemperatureDisplay must handle TemperatureReading");

    thermometer.measure(20);
    wiring.publish(SensorFault{1}); // Each message reaches only receivers with a matching receive().

    return display.lastCelsius == 20 && display.readingsReceived == 1 && faultLog.faultsReceived == 1;
}

bool hideTheWiringType()
{
    TemperatureDisplay display;
    auto wiring = sub0::wire(display);

    // A Sink gives the publisher one fixed output type, regardless of the receiver list.
    // The wiring and its receivers must outlive this non-owning output.
    TemperatureSensor<sub0::Sink<TemperatureReading>> thermometer{sub0::Sink<TemperatureReading>{wiring}};
    thermometer.measure(21);

    return display.lastCelsius == 21 && display.readingsReceived == 1;
}

// StaticWiring encodes receiver addresses in its type, so these objects need static storage.
TemperatureDisplay fixedTemperatureDisplay;
using FixedWiring = sub0::StaticWiring<&fixedTemperatureDisplay>;

bool bindFixedAddresses()
{
    TemperatureSensor<FixedWiring> thermometer{FixedWiring{}};
    thermometer.measure(22);
    return fixedTemperatureDisplay.lastCelsius == 22 && fixedTemperatureDisplay.readingsReceived == 1;
}

bool stopAnOutOfRangeReading()
{
    TemperatureRangeCheck rangeCheck;
    TemperatureDisplay display;
    auto wiring = sub0::wire(rangeCheck, display);

    // publishCancelable stops on false; ordinary publish ignores a receiver's return value.
    wiring.publishCancelable(TemperatureReading{200});
    if (display.readingsReceived != 0)
        return false;

    wiring.publishCancelable(TemperatureReading{23});
    return display.lastCelsius == 23 && display.readingsReceived == 1;
}

int main()
{
    if (!wireLocalReceivers()) return 1;
    if (!hideTheWiringType()) return 2;
    if (!bindFixedAddresses()) return 3;
    if (!stopAnOutOfRangeReading()) return 4;
    return 0;
}

/** Deliver readings locally and to a link, without echoing incoming readings back to that link. */
#include "sub0pub/wiring.hpp"
#include "sub0pub/broker.hpp"

struct TemperatureReading
{
    int celsius;
    // Routes need a publish context to track the incoming link and report send results.
    using sub0_config = sub0::config<sub0::Scoped, sub0::ThreadLocalContext>;
};

struct TelemetryLink
{
    unsigned readingsAccepted = 0;
    bool queueFull = false;

    sub0::SendResult send(const TemperatureReading&) noexcept
    {
        if (queueFull)
            return sub0::SendResult::Full;

        ++readingsAccepted; // A real link copies/serializes here; acceptance is not remote delivery.
        return sub0::SendResult::Accepted;
    }
};

struct TemperatureDisplay
{
    unsigned readingsReceived = 0;
    void receive(const TemperatureReading&) noexcept { ++readingsReceived; }
};

bool forwardWithoutEcho()
{
    TelemetryLink link;
    TemperatureDisplay display;
    sub0::Forward<TelemetryLink> forward(link);
    auto wiring = sub0::wire(display, forward);

    wiring.publish(TemperatureReading{20}); // Outgoing: display and link.
    wiring.publishFrom(link, TemperatureReading{21}); // Incoming: display only.

    // Forward ignores send results. Use a Route when the publisher needs a rejection report.
    return display.readingsReceived == 2 && link.readingsAccepted == 1;
}

// The same forwarding pattern with receiver and transport addresses fixed in the wiring type.
TelemetryLink fixedLink;
TemperatureDisplay fixedDisplay;
sub0::StaticForward<&fixedLink> fixedForward;
using FixedWiring = sub0::StaticWiring<&fixedDisplay, &fixedForward>;

bool forwardThroughFixedAddresses()
{
    FixedWiring::publish(TemperatureReading{20});
    FixedWiring::publishFrom(fixedLink, TemperatureReading{21});
    return fixedDisplay.readingsReceived == 2 && fixedLink.readingsAccepted == 1;
}

struct SubscribedTemperatureDisplay final : sub0::Subscribe<TemperatureReading>
{
    using Subscribe::Subscribe;
    unsigned readingsReceived = 0;
    void receive(const TemperatureReading&) noexcept override { ++readingsReceived; }
};

struct TemperatureSensor final : sub0::Publish<TemperatureReading>
{
    using Publish::Publish;
};

bool reportWhenTheLinkIsFull()
{
    TelemetryLink link;
    sub0::Domain<TemperatureReading> session;
    SubscribedTemperatureDisplay display(session);
    TemperatureSensor thermometer(session);
    sub0::Route<TemperatureReading, TelemetryLink> route(session, link);
    if (!route.isSubscribed()) // A route uses a subscriber slot too.
        return false;

    sub0::PublishReport acceptedReport;
    sub0::publish(thermometer, TemperatureReading{20}, acceptedReport);
    if (acceptedReport.accepted != 1 || display.readingsReceived != 1)
        return false;

    link.queueFull = true;
    sub0::PublishReport rejectedReport;
    sub0::publish(thermometer, TemperatureReading{21}, rejectedReport);
    if (rejectedReport.rejected != 1 || rejectedReport.lastRejection != sub0::SendResult::Full ||
        display.readingsReceived != 2) // Local delivery continues despite the full link.
        return false;

    route.inject(TemperatureReading{22}); // Incoming traffic still reaches the display and is not echoed.
    return display.readingsReceived == 3 && link.readingsAccepted == 1;
}

int main()
{
    if (!forwardWithoutEcho()) return 1;
    if (!forwardThroughFixedAddresses()) return 2;
    if (!reportWhenTheLinkIsFull()) return 3;
    return 0;
}

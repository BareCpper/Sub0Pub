/** Case: static/dynamic bridge with several dynamic subscribers and churn. Two static receivers (controller,
 *  logger); two dynamic probes (offsets 11 and 13) subscribed at setup; a third, transient probe (offset 17)
 *  exists only for publications where (v & 3) == 0 (subscribed before, unsubscribed after). Order:
 *  controller, logger, then dynamic subscribers in subscription order.
 *  Alternative C (inverse direction): the sensor publishes into the #8 registry; the static wiring is one
 *  subscriber (StaticAdapter) ahead of the dynamic probes. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"
#include "sandbox/sub0x_bridge.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Scoped, sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
using Bus = sub0x::StaticWiring<&controller, &logger>;
struct Probe final : sub0x::Subscribe<Sample> {
    Probe(sub0x::Domain<Sample>& d, uint32_t o) noexcept : sub0x::Subscribe<Sample>(d), offset(o) {}
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + offset); }
    uint32_t offset;
};
struct Sensor : sub0x::Publish<Sample> {
    explicit Sensor(sub0x::Domain<Sample>& d) noexcept : sub0x::Publish<Sample>(d) {}
    void send(const Sample& s) noexcept { sub0x::publish(*this, s); }
};
collapse::Slot<sub0x::Domain<Sample>> domain;
collapse::Slot<sub0x::StaticAdapter<Bus, Sample>> adapter;
collapse::Slot<Probe> probeA;
collapse::Slot<Probe> probeB;
collapse::Slot<Sensor> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace(); logger.emplace(); domain.emplace(); adapter.emplace(domain.get());
    probeA.emplace(domain.get(), 11U); probeB.emplace(domain.get(), 13U); sensor.emplace(domain.get());
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v)
{
    const Sample s{collapse::arg(v)};
    if ((s.value & 3U) == 0U)
    {
        Probe transient(domain.get(), 17U);
        sensor->send(s);
    }
    else
        sensor->send(s);
}
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset(); probeB.reset(); probeA.reset(); adapter.reset(); domain.reset(); logger.reset(); controller.reset();
}

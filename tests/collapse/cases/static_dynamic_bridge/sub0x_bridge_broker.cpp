/** Static/dynamic bridge, alternative A: a BrokerPort bound into the static wiring forwards to a full #8
 *  runtime registry (sub0x_broker.hpp: Domain/Subscribe/Publish) for the dynamic subscriber. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"
#include "sandbox/sub0x_bridge.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Scoped>; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct Probe final : sub0x::Subscribe<Sample> {
    explicit Probe(sub0x::Domain<Sample>& d) noexcept : sub0x::Subscribe<Sample>(d) {}
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 11U); }
};

collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<sub0x::Domain<Sample>> domain;
collapse::Slot<sub0x::BrokerPort<Sample>> port;
collapse::Slot<Probe> probe;
using Bus = sub0x::StaticWiring<&controller, &logger, &port>;
template<class Out>
struct Sensor {
    void send(uint32_t v) noexcept { Out::publish(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace();
    logger.emplace();
    domain.emplace();
    port.emplace(domain.get());
    probe.emplace(domain.get());
    sensor.emplace();
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset();
    probe.reset();
    port.reset();
    domain.reset();
    logger.reset();
    controller.reset();
}

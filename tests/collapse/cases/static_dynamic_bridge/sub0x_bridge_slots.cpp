/** Static/dynamic bridge, alternative B: a DynamicPort bound into the static wiring owns a minimal
 *  fixed-capacity slot array itself -- no broker dependency, no policy (tests/collapse/sandbox/sub0x_bridge.hpp). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"
#include "sandbox/sub0x_bridge.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct Probe final : sub0x::DynamicPort<Sample>::Receiver {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 11U); }
};

collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<sub0x::DynamicPort<Sample>> port;
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
    port.emplace();
    probe.emplace();
    port->add(&probe.get());
    sensor.emplace();
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset();
    port->remove(&probe.get());
    probe.reset();
    port.reset();
    logger.reset();
    controller.reset();
}

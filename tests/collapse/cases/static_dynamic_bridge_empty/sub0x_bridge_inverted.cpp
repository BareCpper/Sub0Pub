/** Static/dynamic bridge, alternative C, empty dynamic side: the StaticAdapter is still registered in the
 *  #8 runtime registry (it must be, to keep delivering to the static receivers at all), but no dynamic Probe
 *  is ever added. Checks whether the inverted bridge can collapse when the dynamic side is empty -- it
 *  cannot: every publish still goes through the registry's virtual dispatch just to reach the static side. */
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
collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
using Bus = sub0x::StaticWiring<&controller, &logger>;

collapse::Slot<sub0x::Domain<Sample>> domain;
collapse::Slot<sub0x::StaticAdapter<Bus, Sample>> adapter;

struct Sensor : sub0x::Publish<Sample> {
    explicit Sensor(sub0x::Domain<Sample>& d) noexcept : sub0x::Publish<Sample>(d) {}
    void send(uint32_t v) noexcept { sub0x::publish(*this, Sample{v}); }
};
collapse::Slot<Sensor> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace();
    logger.emplace();
    domain.emplace();
    adapter.emplace(domain.get());
    sensor.emplace(domain.get());
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset();
    adapter.reset();
    domain.reset();
    logger.reset();
    controller.reset();
}

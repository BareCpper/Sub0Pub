/** Static/dynamic bridge, alternative C (inverse direction, for comparison): the sensor publishes into a
 *  full #8 runtime registry; the static wiring is registered as ONE subscriber (StaticAdapter) that forwards
 *  to it, ahead of the dynamic Probe, so all three still fire in controller/logger/probe order. This folds
 *  the always-known static receivers behind the same virtual dispatch as the genuinely dynamic ones. */
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

struct Probe final : sub0x::Subscribe<Sample> {
    explicit Probe(sub0x::Domain<Sample>& d) noexcept : sub0x::Subscribe<Sample>(d) {}
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 11U); }
};
collapse::Slot<sub0x::Domain<Sample>> domain;
collapse::Slot<sub0x::StaticAdapter<Bus, Sample>> adapter;
collapse::Slot<Probe> probe;

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
    probe.emplace(domain.get());
    sensor.emplace(domain.get());
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset();
    probe.reset();
    adapter.reset();
    domain.reset();
    logger.reset();
    controller.reset();
}

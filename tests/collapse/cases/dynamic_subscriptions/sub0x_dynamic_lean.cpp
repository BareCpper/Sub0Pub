/** Case: dynamic subscriptions. The #8 prototype's runtime registry (default configuration). */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; }; // lean: the hand-written registry's features
struct Controller final : sub0x::Subscribe<Sample> {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * 3U); }
};
struct Probe final : sub0x::Subscribe<Sample> {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 11U); }
};
struct Sensor : sub0x::Publish<Sample> {
    void send(const Sample& s) noexcept { sub0x::publish(*this, s); }
};
collapse::Slot<Sensor> sensor;
collapse::Slot<Controller> controller;
}

COLLAPSE_ENTRY void collapse_setup() { sensor.emplace(); controller.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v)
{
    const Sample s{collapse::arg(v)};
    if ((s.value & 3U) == 0U)
    {
        Probe probe;
        sensor->send(s);
    }
    else
        sensor->send(s);
}
COLLAPSE_ENTRY void collapse_teardown() { controller.reset(); sensor.reset(); }

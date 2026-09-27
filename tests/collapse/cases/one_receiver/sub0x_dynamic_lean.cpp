/** Case: one concrete receiver. The #8 prototype's runtime registry (sub0x_broker.hpp) in its leanest valid
 *  configuration for this case, so the binding-form axis compares every form on the same scenarios
 *  (docs/design/COLLAPSE_SCORES.md). Judged against static hand-written code: the collapse question. */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };
struct Controller final : sub0x::Subscribe<Sample> {
    explicit Controller(uint32_t g) noexcept : gain(g) {}
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};
struct Sensor : sub0x::Publish<Sample> {
    void send(uint32_t v) noexcept { sub0x::publish(*this, Sample{v}); }
};
collapse::Slot<Sensor> sensor;
collapse::Slot<Controller> controller;
}

COLLAPSE_ENTRY void collapse_setup() { sensor.emplace(); controller.emplace(3U); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { controller.reset(); sensor.reset(); }

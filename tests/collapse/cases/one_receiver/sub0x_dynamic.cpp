/** Case: one concrete receiver. The #8 prototype's runtime registry (sub0x_broker.hpp), default (Builtin)
 *  configuration: Snapshot, thread-local context, filter, Global storage, capacity 8. */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; };
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

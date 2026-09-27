/** Case: default and runtime filters. The #8 prototype's runtime registry (sub0x_broker.hpp), default (Builtin)
 *  configuration: Snapshot, thread-local context, filter, Global storage, capacity 8. */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller final : sub0x::Subscribe<Sample> {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * 3U); }
};
struct EvenMonitor final : sub0x::Subscribe<Sample> {
    bool filter(const Sample& s) noexcept override { return (s.value & 1U) == 0U; }
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 7U); }
};
struct Sensor : sub0x::Publish<Sample> {
    void send(uint32_t v) noexcept { sub0x::publish(*this, Sample{v}); }
};
collapse::Slot<Sensor> sensor;
collapse::Slot<Controller> controller;
collapse::Slot<EvenMonitor> monitor;
}

COLLAPSE_ENTRY void collapse_setup() { sensor.emplace(); controller.emplace(); monitor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { monitor.reset(); controller.reset(); sensor.reset(); }

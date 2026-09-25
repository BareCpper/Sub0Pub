/** Case: default and runtime filters: Controller declares an always-true filter (must compile away); EvenMonitor's runtime filter keeps its branch.
 *  Pattern B1: typed wiring bound at the composition point; publisher templated on its output (tests/collapse/sandbox/sub0x_static.hpp). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    bool filter(const Sample&) const noexcept { return true; }
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct EvenMonitor {
    bool filter(const Sample& s) const noexcept { return (s.value & 1U) == 0U; }
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value + 7U); }
};

template<class Out>
struct Sensor {
    explicit Sensor(const Out& o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    const Out& out;
};
using Bus = sub0x::Wiring<Controller, EvenMonitor>;
collapse::Slot<Controller> controller;
collapse::Slot<EvenMonitor> monitor;
collapse::Slot<Bus> bus;
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controller.emplace(); monitor.emplace(); bus.emplace(controller.get(), monitor.get()); sensor.emplace(bus.get()); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); bus.reset(); monitor.reset(); controller.reset(); }

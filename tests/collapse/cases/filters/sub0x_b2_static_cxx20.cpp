// SUB0X_STD: c++20
/** Case: default and runtime filters: Controller declares an always-true filter (must compile away);
 *  EvenMonitor's runtime filter keeps its branch.
 *  Pattern B2, C++20 face-off (issue #9 spike: docs/design/spikes/cxx23_upgrade.md): identical to
 *  sub0x_b2_static.cpp except detail::has_filter is a concept (sandbox/sub0x_static_concepts.hpp) instead of
 *  a SFINAE void_t trait. Proves the concept costs nothing extra: same behaviour, same evidence. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static_concepts.hpp"

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

collapse::Slot<Controller> controller;
collapse::Slot<EvenMonitor> monitor;
using Bus = sub0x20::StaticWiring<&controller, &monitor>;
template<class Out>
struct Sensor {
    void send(uint32_t v) noexcept { Out::publish(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controller.emplace(); monitor.emplace(); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); monitor.reset(); controller.reset(); }

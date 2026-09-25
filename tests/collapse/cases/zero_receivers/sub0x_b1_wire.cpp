/** Case: zero receivers.
 *  Pattern B1: typed wiring bound at the composition point; publisher templated on its output (tests/collapse/sandbox/sub0x_static.hpp). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };

template<class Out>
struct Sensor {
    explicit Sensor(const Out& o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    const Out& out;
};
using Bus = sub0x::Wiring<>;
collapse::Slot<Bus> bus;
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { bus.emplace(); sensor.emplace(bus.get()); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); bus.reset(); }

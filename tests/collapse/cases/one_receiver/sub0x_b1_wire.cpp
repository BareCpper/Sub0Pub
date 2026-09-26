// SUB0X_REFERENCE: handwritten_runtime
/** Case: one concrete receiver.
 *  Pattern B1: typed wiring bound at the composition point; publisher templated on its output (tests/collapse/sandbox/sub0x_static.hpp). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    explicit Controller(uint32_t g) noexcept : gain(g) {}
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};

template<class Out>
struct Sensor {
    explicit Sensor(const Out& o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    Out out; // the wiring held by value: a tuple of receiver references, one hop to each receiver
};
using Bus = sub0x::Wiring<Controller>;
collapse::Slot<Controller> controller;
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controller.emplace(3U); sensor.emplace(Bus(controller.get())); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); controller.reset(); }

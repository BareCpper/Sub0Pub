// SUB0X_REFERENCE: handwritten_runtime
/** Cancellation Alt 1 (bool return) on a runtime-bound wiring: Wiring::publishCancelable, added by the scores
 *  review (the spike implemented cancellation for StaticWiring only). Bound order: Gate, Controller, Logger. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    explicit Controller(uint32_t g) noexcept : gain(g) {}
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct Gate {
    bool receive(const Sample& s) noexcept
    {
        COLLAPSE_WORK(s.value);
        return (s.value % 3U) != 0U; // false stops Controller and Logger below
    }
};
template<class Out>
struct Sensor {
    explicit Sensor(const Out& o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publishCancelable(Sample{v}); }
    Out out;
};
using Bus = sub0x::Wiring<Gate, Controller, Logger>;
collapse::Slot<Gate> gate;
collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    gate.emplace(); controller.emplace(3U); logger.emplace();
    sensor.emplace(Bus(gate.get(), controller.get(), logger.get()));
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); logger.reset(); controller.reset(); gate.reset(); }

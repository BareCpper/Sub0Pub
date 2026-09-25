/** Publisher ergonomics face-off (issue #9), alternative 4: composition-point argument. `Sensor` is an
 *  ordinary, non-template class; the output is not stored at all, just passed to the one method that needs
 *  it: `sensor.send(v, bus)`. Nothing about Out appears in the publisher's declaration; the call site chooses
 *  the target every time (so a publisher used from several places can reach different wirings for free, at
 *  the cost of every caller having to have a bus in hand). */
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

// --- what the user writes ---
struct Sensor {
    template<class Out>
    void send(uint32_t v, const Out& out) noexcept { out.publish(Sample{v}); }
};
// call site: sensor.send(v, bus);
// ---

using Bus = sub0x::Wiring<Controller, Controller, Logger>;
collapse::Slot<Controller> controllerA;
collapse::Slot<Controller> controllerB;
collapse::Slot<Logger> logger;
collapse::Slot<Bus> bus;
collapse::Slot<Sensor> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controllerA.emplace(3U); controllerB.emplace(5U); logger.emplace(); bus.emplace(controllerA.get(), controllerB.get(), logger.get()); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v), bus.get()); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); bus.reset(); logger.reset(); controllerB.reset(); controllerA.reset(); }

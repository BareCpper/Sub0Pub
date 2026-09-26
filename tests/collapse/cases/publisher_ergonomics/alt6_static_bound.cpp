/** Publisher ergonomics face-off (issue #9), alternative 6: direct coupling to one static topology. `Sensor`
 *  is an ordinary, non-template class that names the application's `Bus` type directly and calls
 *  `Bus::publish(msg)`. Zero genericity cost (no template anywhere in the publisher), but the publisher's
 *  source now names the application's wiring type, so it cannot be written once in a library and reused
 *  against a different topology without editing it. Included as the "give up reuse, get simplicity" control. */
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

collapse::Slot<Controller> controllerA;
collapse::Slot<Controller> controllerB;
collapse::Slot<Logger> logger;
using Bus = sub0x::StaticWiring<&controllerA, &controllerB, &logger>;

// --- what the user writes ---
struct Sensor {
    void send(uint32_t v) noexcept { Bus::publish(Sample{v}); }
};
// ---

collapse::Slot<Sensor> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controllerA.emplace(3U); controllerB.emplace(5U); logger.emplace(); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); logger.reset(); controllerB.reset(); controllerA.reset(); }

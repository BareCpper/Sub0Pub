// SUB0X_STD: c++20
/** Case: multiple receivers including a repeated type (bound order = delivery order).
 *  Pattern B2, C++20 face-off (issue #9 spike: docs/design/spikes/cxx23_upgrade.md): identical to
 *  sub0x_b2_static.cpp except detail::accepts is a concept (sandbox/sub0x_static_concepts.hpp) instead of a
 *  SFINAE void_t trait. Proves the concept costs nothing extra: same behaviour, same evidence. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static_concepts.hpp"

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
using Bus = sub0x20::StaticWiring<&controllerA, &controllerB, &logger>;
template<class Out>
struct Sensor {
    void send(uint32_t v) noexcept { Out::publish(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controllerA.emplace(3U); controllerB.emplace(5U); logger.emplace(); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); logger.reset(); controllerB.reset(); controllerA.reset(); }

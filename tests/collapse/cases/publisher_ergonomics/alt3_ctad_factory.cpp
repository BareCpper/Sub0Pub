/** Publisher ergonomics face-off (issue #9), alternative 3: CTAD via a factory function. `Sensor` is still a
 *  template over Out, but the *user* never spells `Out` or `Sensor<...>` anywhere: `make_sensor(bus)` deduces
 *  it. Where the application needs a named type (e.g. static storage, as here), it asks the compiler for one
 *  with `decltype(make_sensor(bus))` instead of writing the template argument by hand; on the stack the user
 *  would just write `auto sensor = make_sensor(bus);`. */
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
template<class Out>
struct Sensor {
    explicit Sensor(const Out& o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    const Out& out;
};
template<class Out>
auto make_sensor(const Out& out) noexcept { return Sensor<Out>(out); }
// call site: auto sensor = make_sensor(bus);
// ---

using Bus = sub0x::Wiring<Controller, Controller, Logger>;
collapse::Slot<Controller> controllerA;
collapse::Slot<Controller> controllerB;
collapse::Slot<Logger> logger;
collapse::Slot<Bus> bus;
collapse::Slot<decltype(make_sensor(std::declval<Bus&>()))> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controllerA.emplace(3U); controllerB.emplace(5U); logger.emplace(); bus.emplace(controllerA.get(), controllerB.get(), logger.get()); sensor.emplace(make_sensor(bus.get())); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); bus.reset(); logger.reset(); controllerB.reset(); controllerA.reset(); }

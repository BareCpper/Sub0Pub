// SUB0X_STD: c++23
// SUB0X_REQUIRES: deducing-this
/** Publisher ergonomics face-off (issue #9), C++23 alternative 8: an attempt to get a genuinely
 *  non-template `Sensor` class to zero-cost-publish typed messages using deducing this, to answer the
 *  face-off question directly (docs/design/spikes/cxx23_upgrade.md face-off 1).
 *
 *  Deducing this deduces the *caller's* static type through the explicit object parameter -- here that is
 *  always `Sensor` itself (already concrete, not a template). It gives no new way to learn an unrelated
 *  `Out` type from `self`; `Out` still has to come from somewhere else, so `send` stays a template exactly
 *  as it is in the C++17 call-site-argument form (publisher_ergonomics/alt4_call_site_out.cpp). This variant
 *  is expected to cost the same as alt4, not less -- confirming deducing this changes only how CRTP-shaped
 *  code is *spelled* (alt7), not whether a non-template publisher can reach a concrete bus type for free. */
#include "collapse_case.hpp"
#include <tuple>
#include <utility>

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

// --- what the user writes: not a template at all ---
struct Sensor {
    template<class Out>
    void send(this Sensor& /*self*/, uint32_t v, const Out& out) noexcept { out.publish(Sample{v}); }
};
// ---

template<class... Bound>
class Bus_ {
public:
    constexpr explicit Bus_(Bound&... bound) noexcept : bound_(bound...) {}
    template<class T>
    void publish(const T& msg) const noexcept
    {
        std::apply([&](auto&... b) { ((deliver(b, msg)), ...); }, bound_);
    }
private:
    template<class R, class T>
    static void deliver(R& r, const T& msg) noexcept { r.receive(msg); }
    std::tuple<Bound&...> bound_;
};
using Bus = Bus_<Controller, Controller, Logger>;

collapse::Slot<Controller> controllerA;
collapse::Slot<Controller> controllerB;
collapse::Slot<Logger> logger;
collapse::Slot<Bus> bus;
collapse::Slot<Sensor> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controllerA.emplace(3U); controllerB.emplace(5U); logger.emplace(); bus.emplace(controllerA.get(), controllerB.get(), logger.get()); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v), bus.get()); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); bus.reset(); logger.reset(); controllerB.reset(); controllerA.reset(); }

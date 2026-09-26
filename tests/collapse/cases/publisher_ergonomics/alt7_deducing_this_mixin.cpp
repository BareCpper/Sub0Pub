// SUB0X_STD: c++23
// SUB0X_REQUIRES: deducing-this
// SUB0X_REFERENCE: handwritten_runtime
/** Publisher ergonomics face-off (issue #9), C++23 alternative 7: deducing this (explicit object parameter,
 *  P0847) replacing the C++17 CRTP mixin's `Derived` template parameter (sub0x::Publisher<Derived,Out> in
 *  tests/collapse/sandbox/sub0x_static.hpp -- note that mixin never actually used Derived; it was pure CRTP
 *  boilerplate). Publisher23<Out> below takes only Out; `publish` deduces the caller's own type through the
 *  explicit object parameter, so the derived class no longer has to spell itself in its own base-class list.
 *  Not supported on GCC 13 / arm-none-eabi-g++ 13 (docs/design/spikes/cxx23_upgrade.md); this variant is
 *  skipped by CMake/collapse_evidence.py wherever the compiler lacks deducing this. */
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

// --- sub0x C++23 mixin: no Derived template parameter at all ---
template<class Out>
class Publisher23 {
public:
    constexpr explicit Publisher23(const Out& out) noexcept : out_(out) {}
protected:
    template<class T>
    void publish(this auto&& self, const T& msg) noexcept { self.out_.publish(msg); }
    Out out_; // held by value, as sub0x::Publisher
};

// --- what the user writes ---
template<class Out>
struct Sensor : Publisher23<Out> {
    using Publisher23<Out>::Publisher23;
    void send(uint32_t v) noexcept { this->publish(Sample{v}); }
};
// ---

// minimal B1 wiring (equivalent to sandbox/sub0x_static.hpp's Wiring, reproduced locally so this variant
// does not need C++23 in the shared C++17 sandbox header)
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
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { controllerA.emplace(3U); controllerB.emplace(5U); logger.emplace(); sensor.emplace(Bus(controllerA.get(), controllerB.get(), logger.get())); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); logger.reset(); controllerB.reset(); controllerA.reset(); }

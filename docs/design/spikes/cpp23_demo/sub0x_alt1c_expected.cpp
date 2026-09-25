/** Cancellation Alt 1c (C++23 spike, not wired into the ctest harness which builds at C++17 project-wide):
 *  receive() returns std::expected<void, sub0x::Stop> instead of a bare bool -- a documented reason for
 *  stopping, still by value, still detected at compile time and short-circuited by a fold (Alt 1's
 *  mechanism, tests/collapse/sandbox/sub0x_static.hpp). Controller and Logger are unaffected (void receive()).
 *  Bound order: Gate, Controller, Logger (StaticWiring, pattern B2).
 *  Build manually, e.g.: g++ -std=c++23 -O2 -I tests/collapse -I include tests/collapse/driver.cpp <this file>
 */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Gate {
    std::expected<void, sub0x::Stop> receive(const Sample& s) noexcept
    {
        COLLAPSE_WORK(s.value);
        if ((s.value % 3U) == 0U)
            return std::unexpected(sub0x::Stop::Canceled);
        return {};
    }
};
struct Controller {
    explicit Controller(uint32_t g) noexcept : gain(g) {}
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};

collapse::Slot<Gate> gate;
collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
using Bus = sub0x::StaticWiring<&gate, &controller, &logger>;
template<class Out>
struct Sensor {
    void send(uint32_t v) noexcept { Out::publishCancelableExpected(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { gate.emplace(); controller.emplace(3U); logger.emplace(); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); logger.reset(); controller.reset(); gate.reset(); }

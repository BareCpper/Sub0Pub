/** Cancellation Alt 4 (control, no first-class cancel support): Gate sets a shared flag as a side effect of
 *  its own receive(); Controller and Logger each declare a filter() that checks it, using the *existing*
 *  filter mechanism (tests/collapse/sandbox/sub0x_static.hpp, unmodified — no cancellation helper is used).
 *  Bound order: Gate, Controller, Logger (StaticWiring, pattern B2, plain publish()). Shows what "no first-
 *  class cancel" costs, and what it demands of every later receiver: each one must remember to add the
 *  filter, and still pays a filter() call (not skipped entirely, just made to do nothing). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Stopped { bool value = false; };

struct Gate {
    explicit Gate(Stopped& s) noexcept : stopped(s) {}
    void receive(const Sample& s) noexcept
    {
        COLLAPSE_WORK(s.value);
        stopped.value = ((s.value % 3U) == 0U);
    }
    Stopped& stopped;
};
struct Controller {
    Controller(uint32_t g, const Stopped& s) noexcept : gain(g), stopped(s) {}
    bool filter(const Sample&) const noexcept { return !stopped.value; }
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
    const Stopped& stopped;
};
struct Logger {
    explicit Logger(const Stopped& s) noexcept : stopped(s) {}
    bool filter(const Sample&) const noexcept { return !stopped.value; }
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
    const Stopped& stopped;
};

collapse::Slot<Stopped> stopped;
collapse::Slot<Gate> gate;
collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
using Bus = sub0x::StaticWiring<&gate, &controller, &logger>;
template<class Out>
struct Sensor {
    void send(uint32_t v) noexcept { Out::publish(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    stopped.emplace();
    gate.emplace(stopped.get());
    controller.emplace(3U, stopped.get());
    logger.emplace(stopped.get());
    sensor.emplace();
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset(); logger.reset(); controller.reset(); gate.reset(); stopped.reset();
}

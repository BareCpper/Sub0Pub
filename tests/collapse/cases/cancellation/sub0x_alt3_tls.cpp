/** Cancellation Alt 3: thread-local publish context, mirroring the runtime path's Broker::cancel()
 *  (include/sub0pub/sub0pub.hpp). Gate calls the free function sub0x::cancel() from inside receive();
 *  every bound receiver is checked against it after delivery (tests/collapse/sandbox/sub0x_static.hpp).
 *  Bound order: Gate, Controller, Logger (StaticWiring, pattern B2). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Gate {
    void receive(const Sample& s) noexcept
    {
        COLLAPSE_WORK(s.value);
        if ((s.value % 3U) == 0U)
            sub0x::cancel();
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
    void send(uint32_t v) noexcept { Out::publishCancelableTLS(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup() { gate.emplace(); controller.emplace(3U); logger.emplace(); sensor.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); logger.reset(); controller.reset(); gate.reset(); }

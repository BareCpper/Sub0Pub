// SUB0X_REFERENCE: handwritten_erased
/** Case: 32 receivers of one type. Pattern B3: non-template publisher holding a type-erased Sink<Sample> into a
 *  typed wiring of 32 receivers. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"
#include "many.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    explicit Controller(uint32_t g) noexcept : gain(g) {}
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};
#define MANY_DECLARE(i) collapse::Slot<Controller> c##i;
MANY_EACH(MANY_DECLARE)
struct Sensor {
    explicit Sensor(sub0x::Sink<Sample> o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    sub0x::Sink<Sample> out;
};
using Bus = sub0x::Wiring<MANY_TYPES>;
collapse::Slot<Bus> bus;
collapse::Slot<Sensor> sensor;
}

#define MANY_EMPLACE(i) c##i.emplace(i + 1U);
#define MANY_RESET(i) c##i.reset();
COLLAPSE_ENTRY void collapse_setup() { MANY_EACH(MANY_EMPLACE) bus.emplace(MANY_GETS); sensor.emplace(sub0x::Sink<Sample>(bus.get())); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); bus.reset(); MANY_EACH(MANY_RESET) }

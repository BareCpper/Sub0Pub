/** Case: 32 receivers of one type. The #8 prototype's runtime registry (default configuration),
 *  with capacity 32. */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"
#include "many.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Capacity<32>>; };
struct Controller final : sub0x::Subscribe<Sample> {
    explicit Controller(uint32_t g) noexcept : gain(g) {}
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};
struct Sensor : sub0x::Publish<Sample> {
    void send(uint32_t v) noexcept { sub0x::publish(*this, Sample{v}); }
};
collapse::Slot<Sensor> sensor;
#define MANY_DECLARE(i) collapse::Slot<Controller> c##i;
MANY_EACH(MANY_DECLARE)
}

#define MANY_EMPLACE(i) c##i.emplace(i + 1U);
#define MANY_RESET(i) c##i.reset();
COLLAPSE_ENTRY void collapse_setup() { sensor.emplace(); MANY_EACH(MANY_EMPLACE) }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { MANY_EACH(MANY_RESET) sensor.reset(); }

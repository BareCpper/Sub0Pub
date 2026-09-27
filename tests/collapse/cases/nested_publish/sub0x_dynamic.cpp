/** Case: nested publication. Relay receives Sample, publishes Command{v + 100} and, for odd v, re-publishes
 *  Sample{v + 1} (same type, re-entrant, bounded by the data). Actuator receives Command, Tail (gain 5)
 *  receives Sample. Bound order relay, actuator, tail: the nested publications complete before Tail sees
 *  the outer Sample.
 *  The #8 prototype's runtime registry, default configuration (Snapshot). */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; };
struct Command { uint32_t code; };
struct Relay final : sub0x::Subscribe<Sample>, sub0x::Publish<Sample>, sub0x::Publish<Command> {
    void receive(const Sample& s) noexcept override
    {
        COLLAPSE_WORK(s.value);
        sub0x::publish(static_cast<sub0x::Publish<Command>&>(*this), Command{s.value + 100U});
        if ((s.value & 1U) != 0U)
            sub0x::publish(static_cast<sub0x::Publish<Sample>&>(*this), Sample{s.value + 1U});
    }
};
struct Actuator final : sub0x::Subscribe<Command> {
    void receive(const Command& c) noexcept override { COLLAPSE_WORK(c.code << 1U); }
};
struct Tail final : sub0x::Subscribe<Sample> {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * 5U); }
};
struct Sensor : sub0x::Publish<Sample> {
    void send(uint32_t v) noexcept { sub0x::publish(*this, Sample{v}); }
};
collapse::Slot<Sensor> sensor;
collapse::Slot<Relay> relay;
collapse::Slot<Actuator> actuator;
collapse::Slot<Tail> tail;
}

COLLAPSE_ENTRY void collapse_setup() { sensor.emplace(); relay.emplace(); actuator.emplace(); tail.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { tail.reset(); actuator.reset(); relay.reset(); sensor.reset(); }

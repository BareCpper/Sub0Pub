/** Case: two message types in one wiring. Controller handles Sample and Command, Logger only Sample,
 *  Actuator only Command. Each publication sends Sample{v} then Command{v ^ 0xF}; bound order
 *  controller, logger, actuator, so Sample reaches controller then logger, Command controller then actuator.
 *  The #8 prototype's runtime registry, leanest valid configuration. Controller subscribes to both types (two bases). */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };
struct Command { uint32_t code; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };
struct Controller final : sub0x::Subscribe<Sample>, sub0x::Subscribe<Command> {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * 3U); }
    void receive(const Command& c) noexcept override { COLLAPSE_WORK(c.code + 1U); }
};
struct Logger final : sub0x::Subscribe<Sample> {
    void receive(const Sample& s) noexcept override { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct Actuator final : sub0x::Subscribe<Command> {
    void receive(const Command& c) noexcept override { COLLAPSE_WORK(c.code << 2U); }
};
struct Sensor : sub0x::Publish<Sample>, sub0x::Publish<Command> {
    void send(uint32_t v) noexcept
    {
        sub0x::publish(static_cast<sub0x::Publish<Sample>&>(*this), Sample{v});
        sub0x::publish(static_cast<sub0x::Publish<Command>&>(*this), Command{v ^ 0xFU});
    }
};
collapse::Slot<Sensor> sensor;
collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<Actuator> actuator;
}

COLLAPSE_ENTRY void collapse_setup() { sensor.emplace(); controller.emplace(); logger.emplace(); actuator.emplace(); }
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown() { actuator.reset(); logger.reset(); controller.reset(); sensor.reset(); }

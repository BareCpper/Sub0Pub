/** Case: two independent domains. Dynamic alternative: the #8 prototype's runtime registry with Scoped
 *  storage (sub0x::Domain), virtual subscribers registered at construction. */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Scoped>; };
struct Controller final : sub0x::Subscribe<Sample> {
    Controller(sub0x::Domain<Sample>& d, uint32_t g) noexcept : sub0x::Subscribe<Sample>(d), gain(g) {}
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * gain); }
    uint32_t gain;
};
struct Logger final : sub0x::Subscribe<Sample> {
    explicit Logger(sub0x::Domain<Sample>& d) noexcept : sub0x::Subscribe<Sample>(d) {}
    void receive(const Sample& s) noexcept override { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct Sensor : sub0x::Publish<Sample> {
    explicit Sensor(sub0x::Domain<Sample>& d) noexcept : sub0x::Publish<Sample>(d) {}
    void send(uint32_t v) noexcept { sub0x::publish(*this, Sample{v}); }
};
collapse::Slot<sub0x::Domain<Sample>> domainA;
collapse::Slot<sub0x::Domain<Sample>> domainB;
collapse::Slot<Controller> controllerA;
collapse::Slot<Logger> loggerA;
collapse::Slot<Controller> controllerB;
collapse::Slot<Sensor> sensorA;
collapse::Slot<Sensor> sensorB;
}

COLLAPSE_ENTRY void collapse_setup()
{
    domainA.emplace(); domainB.emplace();
    controllerA.emplace(domainA.get(), 3U); loggerA.emplace(domainA.get()); controllerB.emplace(domainB.get(), 5U);
    sensorA.emplace(domainA.get()); sensorB.emplace(domainB.get());
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v)
{
    sensorA->send(collapse::arg(v));
    sensorB->send(collapse::arg(v + 1U));
}
COLLAPSE_ENTRY void collapse_teardown()
{
    sensorB.reset(); sensorA.reset();
    controllerB.reset(); loggerA.reset(); controllerA.reset();
    domainB.reset(); domainA.reset();
}

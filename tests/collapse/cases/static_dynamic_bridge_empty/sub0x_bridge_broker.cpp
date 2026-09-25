/** Static/dynamic bridge, alternative A, empty dynamic side: the BrokerPort and its Domain are still bound
 *  and constructed, but no dynamic subscriber is ever added. Checks whether an unused #8 registry collapses. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"
#include "sandbox/sub0x_bridge.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Scoped>; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};

collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<sub0x::Domain<Sample>> domain;
collapse::Slot<sub0x::BrokerPort<Sample>> port;
using Bus = sub0x::StaticWiring<&controller, &logger, &port>;
template<class Out>
struct Sensor {
    void send(uint32_t v) noexcept { Out::publish(Sample{v}); }
};
collapse::Slot<Sensor<Bus>> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace();
    logger.emplace();
    domain.emplace();
    port.emplace(domain.get());
    sensor.emplace();
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset();
    port.reset();
    domain.reset();
    logger.reset();
    controller.reset();
}

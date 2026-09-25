/** Case: concrete transport endpoint. Pattern B1: typed wiring with a typed Forward<Radio> endpoint binding;
 *  ingress via publishFrom(endpoint) skips the endpoint (split horizon). */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Radio {
    void send(const Sample& s) noexcept { collapse::io(s.value); }
};
using Uplink = sub0x::Forward<Radio>;
using Bus = sub0x::Wiring<Controller, Uplink>;
collapse::Slot<Controller> controller;
collapse::Slot<Radio> radio;
collapse::Slot<Uplink> uplink;
collapse::Slot<Bus> bus;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace(); radio.emplace(); uplink.emplace(radio.get());
    bus.emplace(controller.get(), uplink.get());
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v)
{
    const Sample out{collapse::arg(v)};
    bus->publish(out);
    bus->publishFrom(uplink.get(), Sample{out.value ^ 0x55U});
}
COLLAPSE_ENTRY void collapse_teardown() { bus.reset(); uplink.reset(); radio.reset(); controller.reset(); }

/** Case: concrete transport endpoint. Dynamic alternative: the #8 prototype's runtime registry with a
 *  sub0x::Route<Sample, RadioPort> endpoint binding (split horizon on ingress via Route::inject). */
#include "collapse_case.hpp"
#include "sub0x_broker.hpp"

namespace {
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::StaticContext, sub0x::NoFilter>; }; // lean: routes need a publish context; StaticContext avoids TLS
struct Controller final : sub0x::Subscribe<Sample> {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value * 3U); }
};
struct Radio {
    void send(const Sample& s) noexcept { collapse::io(s.value); }
};
struct RadioPort {   // adapts Radio to the prototype's Transport concept
    explicit RadioPort(Radio& r) noexcept : radio(r) {}
    sub0x::SendResult send(const Sample& s) noexcept { radio.send(s); return sub0x::SendResult::Accepted; }
    Radio& radio;
};
struct Sensor : sub0x::Publish<Sample> {
    void send(const Sample& s) noexcept { sub0x::publish(*this, s); }
};
using Uplink = sub0x::Route<Sample, RadioPort>;
collapse::Slot<Controller> controller;
collapse::Slot<Radio> radio;
collapse::Slot<RadioPort> port;
collapse::Slot<Uplink> uplink;
collapse::Slot<Sensor> sensor;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace(); radio.emplace(); port.emplace(radio.get()); uplink.emplace(port.get()); sensor.emplace();
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v)
{
    const Sample out{collapse::arg(v)};
    sensor->send(out);
    uplink->inject(Sample{out.value ^ 0x55U});
}
COLLAPSE_ENTRY void collapse_teardown() { sensor.reset(); uplink.reset(); port.reset(); radio.reset(); controller.reset(); }

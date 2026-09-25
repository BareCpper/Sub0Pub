/** Static/dynamic bridge, C++23 variant of alternative B: same DynamicPort as sub0x_bridge_slots.cpp, but the
 *  bind-time call reports capacity failure through std::expected instead of silently dropping it, where the
 *  toolchain provides it (feature-tested; GCC 13's libstdc++ has it, Clang 18 + that same libstdc++ does not --
 *  __cpp_lib_expected is gated on __cpp_concepts >= 202002L, which Clang 18 does not report -- so this file
 *  still compiles there, it just falls back to the plain bool). See docs/design/spikes/static_dynamic_bridge.md
 *  ("C++23"). Built at -std=c++23 (tests/collapse/CMakeLists.txt bumps this one variant); behaviour, and thus
 *  the checksum, is identical to sub0x_bridge_slots.cpp -- only the bind-time reporting differs. */
#include "collapse_case.hpp"
#include "sandbox/sub0x_static.hpp"
#include "sandbox/sub0x_bridge.hpp"

#if defined(__cpp_lib_expected)
#include <expected>
#endif

namespace demo {
enum class BridgeError { CapacityExceeded };

#if defined(__cpp_lib_expected)
template<class Port, class R>
std::expected<void, BridgeError> addChecked(Port& port, R* r) noexcept
{
    if (port.tryAdd(r))
        return {};
    return std::unexpected(BridgeError::CapacityExceeded);
}
#else
template<class Port, class R>
bool addChecked(Port& port, R* r) noexcept { return port.tryAdd(r); }
#endif
}

namespace {
struct Sample { uint32_t value; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct Probe final : sub0x::DynamicPort<Sample>::Receiver {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 11U); }
};

collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<sub0x::DynamicPort<Sample>> port;
collapse::Slot<Probe> probe;
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
    port.emplace();
    probe.emplace();
    (void)demo::addChecked(port.get(), &probe.get());
    sensor.emplace();
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v) { sensor->send(collapse::arg(v)); }
COLLAPSE_ENTRY void collapse_teardown()
{
    sensor.reset();
    port->remove(&probe.get());
    probe.reset();
    port.reset();
    logger.reset();
    controller.reset();
}

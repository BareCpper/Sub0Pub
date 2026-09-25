/** Case: static/dynamic bridge (issue #9 open item). A sensor publishes Sample to two static receivers
 *  (controller gain 3, logger) and to a runtime registry holding one dynamic subscriber (Probe), subscribed
 *  during setup. Order: controller, logger, then dynamic subscribers. Reference: direct calls to controller
 *  and logger, plus a minimal hand-written dynamic registry (fixed array + virtual receive) for the probe. */
#include "collapse_case.hpp"

namespace {
struct Sample { uint32_t value; };
struct Controller {
    void receive(const Sample& s) noexcept { COLLAPSE_WORK(s.value * 3U); }
};
struct Logger {
    void receive(const Sample& s) noexcept { ++count; COLLAPSE_WORK(s.value ^ count); }
    uint32_t count = 0;
};
struct DynReceiver {
    virtual void receive(const Sample&) noexcept = 0;
protected:
    ~DynReceiver() = default;
};
struct Registry {
    DynReceiver* entries[4] = {};
    uint32_t count = 0;
    void add(DynReceiver* r) noexcept { if (count < 4) entries[count++] = r; }
    void publish(const Sample& s) const noexcept { for (uint32_t i = 0; i < count; ++i) entries[i]->receive(s); }
};
struct Probe final : DynReceiver {
    void receive(const Sample& s) noexcept override { COLLAPSE_WORK(s.value + 11U); }
};

collapse::Slot<Controller> controller;
collapse::Slot<Logger> logger;
collapse::Slot<Registry> registry;
collapse::Slot<Probe> probe;
}

COLLAPSE_ENTRY void collapse_setup()
{
    controller.emplace();
    logger.emplace();
    registry.emplace();
    probe.emplace();
    registry->add(&probe.get());
}
COLLAPSE_ENTRY void collapse_publish(uint32_t v)
{
    const Sample s{collapse::arg(v)};
    controller->receive(s);
    logger->receive(s);
    registry->publish(s);
}
COLLAPSE_ENTRY void collapse_teardown()
{
    probe.reset();
    registry.reset();
    logger.reset();
    controller.reset();
}

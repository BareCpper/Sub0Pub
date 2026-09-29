/** Fixed topology: plain receivers, runtime or static addresses, erased boundary, cancellation. */
#include "sub0pub/wiring.hpp"

struct Sample { unsigned value; };
struct Alarm { unsigned code; };
struct Display
{
    unsigned total = 0;
    void receive(const Sample& sample) noexcept { total += sample.value; }
};
struct AlarmLog
{
    unsigned count = 0;
    void receive(const Alarm&) noexcept { ++count; }
};
struct Gate
{
    bool receive(const Sample& sample) noexcept { return sample.value < 10U; }
};

template<class Out>
struct Sensor final : sub0::Publisher<Sensor<Out>, Out>
{
    using sub0::Publisher<Sensor<Out>, Out>::Publisher;
    void sample(unsigned value) noexcept { this->publish(Sample{value}); }
};

// StaticWiring needs objects with static storage, not stack-local addresses.
Display fixedDisplay;
using FixedBus = sub0::StaticWiring<&fixedDisplay>;

int main()
{
    Display display;
    AlarmLog alarms;
    auto bus = sub0::wire(display, alarms);
    static_assert(sub0::handles_v<Display, Sample>, "Display must handle Sample");
    Sensor<decltype(bus)> sensor(bus);
    sensor.sample(3);
    bus.publish(Alarm{1}); // Non-matching receivers are skipped.

    // The wiring and all of its receivers must outlive the Sink and publisher.
    Sensor<sub0::Sink<Sample>> erased{sub0::Sink<Sample>{bus}};
    erased.sample(4);
    Sensor<FixedBus> fixed{FixedBus{}};
    fixed.sample(5);

    Gate gate;
    auto checked = sub0::wire(gate, display);
    checked.publishCancelable(Sample{20}); // false stops later receivers.
    checked.publishCancelable(Sample{2});
    // Plain publish() ignores a bool result; use publishCancelable() deliberately.
    return display.total == 9 && alarms.count == 1 && fixedDisplay.total == 5 ? 0 : 1;
}

// Uses each entry header of an installed Sub0Pub: the runtime broker and the static wiring
#include <sub0pub/broker.hpp>
#include <sub0pub/wiring.hpp>

struct Reading { int value; };

struct Total final : sub0::Subscribe<Reading>
{
    int sum = 0;
    void receive(const Reading& reading) noexcept override { sum += reading.value; }
};

struct Source final : sub0::Publish<Reading>
{
    void send(int value) noexcept { sub0::publish(*this, Reading{value}); }
};

struct Display
{
    int last = 0;
    void receive(const Reading& reading) noexcept { last = reading.value; }
};

int main()
{
    Total total;
    Source source;
    source.send(2);

    Display display;
    auto wiring = sub0::wire(display);
    wiring.publish(Reading{3});

    return total.isSubscribed() && total.sum == 2 && display.last == 3 ? 0 : 1;
}

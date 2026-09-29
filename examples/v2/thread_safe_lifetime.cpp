/** Two clocks publish from separate threads to one counter. Register only after construction; disconnect before destruction. */
#include "sub0pub/broker.hpp"
#include <atomic>
#include <mutex>
#include <thread>

struct Tick
{
    using sub0_config = sub0::config<sub0::LockWith<std::mutex>>;
};

struct Clock final : sub0::Publish<Tick>
{
    void tick() noexcept { sub0::publish(*this, Tick{}); }
};

struct TickCounter final : sub0::Subscribe<Tick>
{
    std::atomic<unsigned> ticksReceived{0};

    TickCounter() noexcept
    {
        // Locked subscribers do not auto-register: all derived state must be ready first.
        trySubscribe();
    }

    ~TickCounter()
    {
        disconnect(); // Wait for in-flight callbacks before destroying derived state.
    }

    void receive(const Tick&) noexcept override
    {
        ++ticksReceived; // Broker locking protects its table; callbacks can run concurrently.
    }
};

int main()
{
    TickCounter counter;
    if (!counter.isSubscribed())
        return 1;

    const auto publishTicks = []
    {
        Clock clock;
        for (unsigned i = 0; i < 100; ++i)
            clock.tick();
    };

    std::thread firstClock(publishTicks);
    std::thread secondClock(publishTicks);
    firstClock.join();
    secondClock.join();

    return counter.ticksReceived.load() == 200 ? 0 : 2;
}

/** Spike (#5), C++20 outlook: mechanism 2b (refcount + std::atomic wait/notify) face-off against the
 * C++17 mechanisms, same Guarded cross-thread teardown test and starvation measurement. Kept in its own
 * TU/target so the C++17 baseline never depends on C++20. See docs/design/spikes/quiescence.md.
 */
#include "doctest.h"
#include "qx_refcount_wait.hpp"

#include <atomic>
#include <memory>
#include <thread>

#if defined(__cpp_lib_atomic_wait)

namespace {

struct SharedRcw { int seq; };
struct StarveRcw { int seq; };

template<class Base, class Data>
struct Guarded : Base
{
    Guarded() : counter(new int(0)) { this->activate(); } // K5: activate only once fully constructed
    ~Guarded() override { this->disconnect(); delete counter; counter = nullptr; }
    void receive(const Data&) noexcept override
    {
        for (volatile int spin = 0; spin < 200; ++spin) {}
        ++*counter;
    }
    int* counter;
};

template<class Base, class Data>
struct Idle : Base
{
    Idle() { this->activate(); }
    // Lifetime contract: disconnect() FIRST in the most-derived destructor (see test_quiescence.cpp).
    ~Idle() override { this->disconnect(); }
    void receive(const Data&) noexcept override {}
};

} // namespace

TEST_CASE("quiescence: mechanism 2b (refcount + C++20 atomic wait/notify) teardown during delivery, cross-thread")
{
    using GuardedT = Guarded<qx::rcw::Subscribe<SharedRcw>, SharedRcw>;
    std::atomic<bool> stop{false};
    std::atomic<int> published{0};

    std::thread publisher([&] {
        while (!stop.load(std::memory_order_relaxed))
            qx::rcw::publish(SharedRcw{published.fetch_add(1, std::memory_order_relaxed)});
    });

    for (int i = 0; i < 2000; ++i)
    {
        GuardedT a;
        auto b = std::make_unique<GuardedT>();
        std::this_thread::yield();
    }

    stop.store(true, std::memory_order_relaxed);
    publisher.join();
    CHECK(published.load() > 0);
}

TEST_CASE("quiescence: mechanism 2b starvation bound (wake-ups, not spins) under continuous publishing")
{
    using IdleT = Idle<qx::rcw::Subscribe<StarveRcw>, StarveRcw>;
    std::atomic<bool> stop{false};
    std::thread publisher([&] {
        while (!stop.load(std::memory_order_relaxed))
            qx::rcw::publish(StarveRcw{0});
    });

    uint64_t maxIters = 0;
    for (int i = 0; i < 500; ++i)
    {
        qx::waitIterCounter().store(0, std::memory_order_relaxed);
        { IdleT sub; (void)sub; }
        maxIters = std::max(maxIters, qx::waitIterCounter().load(std::memory_order_relaxed));
    }

    stop.store(true, std::memory_order_relaxed);
    publisher.join();
    MESSAGE("mechanism 2b: max disconnect() wake-ups over 500 rounds: ", maxIters);
    CHECK(maxIters < 1000);
}

#else
TEST_CASE("quiescence: C++20 atomic wait/notify not available, skipped") {}
#endif

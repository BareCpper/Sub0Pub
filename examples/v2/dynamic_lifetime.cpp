/** Migrating v1 lifetimes: bounded registration, opt-in filter/cancel, snapshot mutation, locked teardown. */
#include "sub0pub/broker.hpp"
#include <atomic>
#include <mutex>

struct Sample
{
    unsigned value;
    using sub0_config = sub0::config<sub0::Scoped, sub0::Capacity<2>, sub0::Snapshot, sub0::Filter>;
};
struct Source final : sub0::Publish<Sample> { using Publish::Publish; };
struct Observer final : sub0::Subscribe<Sample>
{
    using Subscribe::Subscribe;
    unsigned count = 0;
    bool stop = false;
    bool detach = false;
    bool filter(const Sample& sample) noexcept override { return sample.value != 0; }
    void receive(const Sample&) noexcept override
    {
        ++count;
        if (stop) cancel();
        if (detach) disconnect(); // Requires Snapshot; Direct cannot mutate its active table.
    }
};

struct LockedSample
{
    using sub0_config = sub0::config<sub0::LockWith<std::mutex>>;
};
struct LockedSource final : sub0::Publish<LockedSample> {};
struct LockedObserver final : sub0::Subscribe<LockedSample>
{
    std::atomic<unsigned> count{0}; // Several publishing threads may enter receive().
    LockedObserver() noexcept { trySubscribe(); } // Most-derived construction is complete.
    ~LockedObserver() { disconnect(); } // Wait for callbacks before derived state is destroyed.
    void receive(const LockedSample&) noexcept override { ++count; }
};

int main()
{
    sub0::Domain<Sample> domain; // Outlives every publisher/subscriber bound to it.
    Source source(domain);
    Observer first(domain), second(domain), overflow(domain);
    if (overflow.trySubscribe() != sub0::SubscribeResult::CapacityExceeded) return 1;
    sub0::publish(source, Sample{0}); // Both filters reject.
    first.stop = true;
    sub0::publish(source, Sample{1}); // Only first receives.
    first.stop = false;
    first.detach = true;
    sub0::publish(source, Sample{1}); // Self-removal does not skip second.
    if (overflow.trySubscribe() != sub0::SubscribeResult::Subscribed) return 2;
    sub0::publish(source, Sample{1});
    if (first.count != 2 || second.count != 2 || overflow.count != 1) return 3;
    domain.close();
    sub0::publish(source, Sample{1}); // Closed domain drops publication.
    if (overflow.trySubscribe() != sub0::SubscribeResult::Closed || overflow.count != 1) return 4;

    LockedSource lockedSource;
    LockedObserver lockedObserver;
    sub0::publish(lockedSource, LockedSample{});
    return lockedObserver.count.load() == 1 ? 0 : 5;
}

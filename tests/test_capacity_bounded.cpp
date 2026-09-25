/** Regression tests for GitHub issue #4:
 *  "Release-mode capacity failure: subscriber table overflow is assertion-only"
 *
 * Overrides SUB0PUB_MAX_SUBSCRIPTIONS to a small value for this translation unit
 * so the bounded capacity-exceeded path can be exercised deterministically.
 * Assertions are left at their default: capacity exhaustion is a reported outcome
 * (SubscribeResult::CapacityExceeded) in debug and release builds alike. CI runs
 * this suite under ASan/UBSan both with and without NDEBUG (ci-asan, ci-asan-release).
 *
 * CapMsg is a type unique to this translation unit (internal linkage via the
 * anonymous namespace), so its Broker<CapMsg> monostate state_ is not shared
 * with any other test file's Broker<T> instantiation.
 */
#define SUB0PUB_MAX_SUBSCRIPTIONS 3

#include <optional>

#include "doctest.h"
#include "sub0pub/sub0pub.hpp"

namespace {

struct CapMsg { int value; };

struct CapSubscriber : sub0::Subscribe<CapMsg>
{
    int received = -1;
    void receive(const CapMsg& data) noexcept override { received = data.value; }
};

struct CapPublisher : sub0::Publish<CapMsg>
{
    void send(int value) { sub0::publish(this, CapMsg{value}); }
};

} // namespace

TEST_CASE("Subscribing up to capacity succeeds (issue #4)") {
    CapSubscriber s0, s1, s2; // cMaxSubscriptions == 3
    CHECK(s0.isSubscribed());
    CHECK(s1.isSubscribed());
    CHECK(s2.isSubscribed());
}

TEST_CASE("Subscribing past capacity fails predictably, table unchanged, existing subscribers unaffected (issue #4)") {
    CapSubscriber s0, s1, s2; // fills capacity (3/3)
    CapSubscriber overflow;   // 4th — over capacity

    CHECK(s0.isSubscribed());
    CHECK(s1.isSubscribed());
    CHECK(s2.isSubscribed());
    CHECK_FALSE(overflow.isSubscribed());

    CapPublisher pub;
    pub.send(7);

    // Acceptance criteria (#4): original N subscribers still receive the event once each.
    CHECK(s0.received == 7);
    CHECK(s1.received == 7);
    CHECK(s2.received == 7);
    // The rejected subscriber was never registered, so it never receives.
    CHECK(overflow.received == -1);
}

TEST_CASE("trySubscribe() reports CapacityExceeded without mutating the table (issue #4)") {
    CapSubscriber s0, s1, s2; // fills capacity (3/3)
    CapSubscriber s3;
    CHECK_FALSE(s3.isSubscribed());

    // Explicit retry while still full: explicit error, table untouched
    CHECK(s3.trySubscribe() == sub0::SubscribeResult::CapacityExceeded);
    CHECK_FALSE(s3.isSubscribed());

    // Already-registered subscriber: idempotent, no duplicate entry
    CHECK(s0.trySubscribe() == sub0::SubscribeResult::Subscribed);

    CapPublisher pub;
    pub.send(99);
    CHECK(s0.received == 99);
    CHECK(s1.received == 99);
    CHECK(s2.received == 99);
    CHECK(s3.received == -1);
}

TEST_CASE("Each subscriber receives exactly once, including after an idempotent trySubscribe() (issue #4)") {
    struct CountingSubscriber : sub0::Subscribe<CapMsg>
    {
        int count = 0;
        void receive(const CapMsg&) noexcept override { ++count; }
    };

    CountingSubscriber a, b, c;
    CHECK(a.trySubscribe() == sub0::SubscribeResult::Subscribed);
    CountingSubscriber overflow;
    CHECK_FALSE(overflow.isSubscribed());

    CapPublisher pub;
    pub.send(1);
    CHECK(a.count == 1);
    CHECK(b.count == 1);
    CHECK(c.count == 1);
    CHECK(overflow.count == 0);
}

TEST_CASE("trySubscribe() reclaims a slot freed by another subscriber (issue #4)") {
    CapSubscriber a, b;       // 2/3 used
    {
        CapSubscriber c;      // 3/3 used
        CHECK(c.isSubscribed());
        CapSubscriber rejected;
        CHECK_FALSE(rejected.isSubscribed());
        CHECK(rejected.trySubscribe() == sub0::SubscribeResult::CapacityExceeded);
    } // c destroyed -> 2/3 used

    std::optional<CapSubscriber> late{std::in_place}; // takes the freed slot at construction
    CHECK(late->isSubscribed());

    CapSubscriber retry;      // full again
    CHECK_FALSE(retry.isSubscribed());
    late.reset();             // release a slot
    CHECK(retry.trySubscribe() == sub0::SubscribeResult::Subscribed);
    CHECK(retry.isSubscribed());

    CapPublisher pub;
    pub.send(5);
    CHECK(a.received == 5);
    CHECK(b.received == 5);
    CHECK(retry.received == 5);
}

TEST_CASE("Unsubscribe recovers capacity for a later registration (issue #4)") {
    CapSubscriber a, b; // 2/3 used
    {
        CapSubscriber c; // 3/3 used
        CHECK(c.isSubscribed());

        CapSubscriber overflow; // rejected — table full
        CHECK_FALSE(overflow.isSubscribed());
    } // c destructs -> unsubscribe() recovers its slot -> 2/3 used

    CapSubscriber recovered;
    CHECK(recovered.isSubscribed());

    CapPublisher pub;
    pub.send(3);
    CHECK(a.received == 3);
    CHECK(b.received == 3);
    CHECK(recovered.received == 3);
}

TEST_CASE("Destroying a never-subscribed (capacity-exceeded) subscriber is a safe no-op (issue #4)") {
    CapSubscriber s0, s1, s2; // fills capacity
    {
        CapSubscriber overflow;
        CHECK_FALSE(overflow.isSubscribed());
        // ~CapSubscriber() must not corrupt state_ here: it was never in the table.
    }
    // Capacity should still read as full (3/3): a broken unsubscribe() would have
    // wrongly freed a slot belonging to s0/s1/s2 when 'overflow' was destroyed.
    CapSubscriber stillOverflow;
    CHECK_FALSE(stillOverflow.isSubscribed());
}

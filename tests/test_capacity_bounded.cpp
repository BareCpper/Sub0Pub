/** Regression tests for GitHub issue #4:
 *  "Release-mode capacity failure: subscriber table overflow is assertion-only"
 *
 * Overrides SUB0PUB_MAX_SUBSCRIPTIONS to a small value and disables the debug
 * assertion (SUB0PUB_ASSERT=0) for this translation unit only, so the bounded
 * capacity-exceeded path added to Broker::trySubscribe() can be exercised
 * deterministically as part of the normal (green) doctest suite, without
 * needing a separate NDEBUG/ASan build. The private ASan repro in
 * repro_issue4_capacity_overflow.cpp additionally proves the pre-fix
 * behaviour corrupted memory under a real NDEBUG build.
 *
 * CapMsg is a type unique to this translation unit (internal linkage via the
 * anonymous namespace), so its Broker<CapMsg> monostate state_ is not shared
 * with any other test file's Broker<T> instantiation.
 */
#define SUB0PUB_ASSERT 0
#define SUB0PUB_MAX_SUBSCRIPTIONS 3

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

TEST_CASE("Direct trySubscribe() reports CapacityExceeded without mutating the table (issue #4)") {
    CapSubscriber s0, s1, s2; // fills capacity (3/3)

    // A raw trySubscribe() call (bypassing the Subscribe<Data> base-class construction path)
    // must also report the explicit error and touch nothing.
    CapSubscriber s3;
    CHECK_FALSE(s3.isSubscribed());

    CapPublisher pub;
    pub.send(99);
    CHECK(s0.received == 99);
    CHECK(s1.received == 99);
    CHECK(s2.received == 99);
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

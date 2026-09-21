/** Standalone repro for GitHub issue #4:
 *  "Release-mode capacity failure: subscriber table overflow is assertion-only"
 *
 * This is NOT part of the doctest suite (Sub0Pub_Tests) on purpose: it is
 * expected to corrupt memory / crash under ASan, which would poison the
 * normal green test run. It exists to demonstrate the bug for the issue,
 * and should be deleted once issue #4 has a real regression test that
 * exercises the *fixed* bounded-capacity API instead.
 *
 * Build (matches a real NDEBUG release build, with the fixed table shrunk
 * to make the (N+1)th subscription trivial to trigger):
 *
 *   clang++ -std=c++17 -DNDEBUG -DSUB0PUB_MAX_SUBSCRIPTIONS=2 \
 *       -fsanitize=address,undefined -fno-omit-frame-pointer -g -O0 \
 *       -I ../include repro_issue4_capacity_overflow.cpp -o repro_issue4
 *
 * Expected (current, unfixed) behaviour: ASan reports a global-buffer-overflow
 * (or, once publish() is called, a stack-buffer-overflow when the oversized
 * count is used to fill the fixed-size snapshot array) instead of a clean,
 * catchable capacity error. Without ASan the process may appear to "work"
 * while silently corrupting adjacent static/stack memory -- that silent
 * corruption is exactly what issue #4 is about.
 */
#include "sub0pub/sub0pub.hpp"

#include <cstdio>

namespace {

// Unique message type so this TU's Broker<CapacityProbe> state is not
// shared with any other test binary's instantiation of Broker<int> etc.
struct CapacityProbe { int value; };

struct ProbeSubscriber : sub0::Subscribe<CapacityProbe>
{
    int received = 0;
    void receive(const CapacityProbe& data) noexcept override
    {
        received = data.value;
    }
};

} // namespace

int main()
{
    static_assert(sub0::detail::Broker<CapacityProbe>::cMaxSubscriptions == 2,
                  "Build with -DSUB0PUB_MAX_SUBSCRIPTIONS=2 for this repro");

    printf("cMaxSubscriptions = %u\n",
           sub0::detail::Broker<CapacityProbe>::cMaxSubscriptions);

    // Fill the table to capacity -- these two are fine.
    ProbeSubscriber s0;
    ProbeSubscriber s1;

    // This third subscription exceeds cMaxSubscriptions (2). Check::onSubscription
    // only *asserts* subscriptionCount < subscriptionCapacity; with NDEBUG defined
    // (as in any normal release build) assert() is a no-op, so nothing stops
    // Broker::Broker(Subscribe<Data>*) from executing
    //     state_.subscriptions[state_.subscriptionCount++] = subscriber;
    // at sub0pub.hpp:363 with subscriptionCount already == cMaxSubscriptions --
    // an out-of-bounds write to the fixed `subscriptions[cMaxSubscriptions]` table.
    printf("Subscribing 3rd subscriber past capacity (this was the OOB write pre-fix)...\n");
    ProbeSubscriber s2;
    printf("s0.isSubscribed=%d s1.isSubscribed=%d s2.isSubscribed=%d\n",
           s0.isSubscribed(), s1.isSubscribed(), s2.isSubscribed());

    // If we get here without ASan aborting, the corruption was silent.
    // A subsequent publish() then walks a corrupted subscriptionCount (3
    // instead of the true capacity 2) and, under SUB0PUB_REENTRANT_SAFE
    // (default on), copies that many pointers into a `snapshot[cMaxSubscriptions]`
    // *stack* array in Broker::publish -- a second, independent OOB write.
    struct Pub : sub0::Publish<CapacityProbe> {
        void send(int v) { sub0::publish(this, CapacityProbe{v}); }
    } pub;

    printf("Publishing (exercises the corrupted subscriptionCount)...\n");
    pub.send(42);

    printf("s0=%d s1=%d s2=%d (no crash/ASan report => corruption was silent)\n",
           s0.received, s1.received, s2.received);
    return 0;
}

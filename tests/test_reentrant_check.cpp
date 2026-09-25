/** Tests for SUB0PUB_REENTRANT_CHECK: detection of re-entrancy in the unguarded
 *  (SUB0PUB_REENTRANT_SAFE=false) direct-iteration dispatch mode.
 *
 * The violation handler is overridden to count instead of abort. Types are unique
 * to this translation unit (anonymous namespace) so their Broker<T> instantiations
 * are not shared with other test files that use the default configuration.
 */
#ifndef SUB0PUB_THREAD_SAFE // THREAD_SAFE forces the snapshot path; the check is then inactive
#define SUB0PUB_REENTRANT_SAFE false
#define SUB0PUB_REENTRANT_CHECK true
#endif

namespace { int gViolations = 0; }
#define SUB0PUB_REENTRANT_VIOLATION(what) (void)(what), ++gViolations

#include <optional>

#include "doctest.h"
#include "sub0pub/sub0pub.hpp"

namespace {

constexpr bool cCheckActive = SUB0PUB_REENTRANT_CHECK && !(SUB0PUB_REENTRANT_SAFE || SUB0PUB_THREAD_SAFE);

struct ReMsg { int value; };
struct OtherMsg { int value; };

struct RePublisher : sub0::Publish<ReMsg>
{
    void send(int value) { sub0::publish(*this, ReMsg{value}); }
};

struct OtherPublisher : sub0::Publish<OtherMsg>
{
    void send(int value) { sub0::publish(*this, OtherMsg{value}); }
};

struct PlainSubscriber : sub0::Subscribe<ReMsg>
{
    int received = 0;
    void receive(const ReMsg&) noexcept override { ++received; }
};

} // namespace

TEST_CASE("Reentrant check: normal publish is not a violation") {
    gViolations = 0;
    RePublisher pub;
    PlainSubscriber a, b;
    pub.send(1);
    CHECK(a.received == 1);
    CHECK(b.received == 1);
    CHECK(gViolations == 0);
}

TEST_CASE("Reentrant check: publishing the same type from receive() is detected") {
    gViolations = 0;
    struct Echo : sub0::Subscribe<ReMsg>
    {
        RePublisher* pub = nullptr;
        void receive(const ReMsg& m) noexcept override { if (m.value > 0) pub->send(m.value - 1); }
    };
    RePublisher pub;
    Echo echo;
    echo.pub = &pub;
    pub.send(1);
    CHECK(gViolations == (cCheckActive ? 1 : 0));
}

TEST_CASE("Reentrant check: publishing a different type from receive() is allowed") {
    gViolations = 0;
    struct Relay : sub0::Subscribe<ReMsg>
    {
        OtherPublisher other;
        void receive(const ReMsg& m) noexcept override { other.send(m.value); }
    };
    struct OtherSub : sub0::Subscribe<OtherMsg>
    {
        int received = 0;
        void receive(const OtherMsg&) noexcept override { ++received; }
    };
    RePublisher pub;
    Relay relay;
    OtherSub sink;
    pub.send(4);
    CHECK(sink.received == 1);
    CHECK(gViolations == 0);
}

TEST_CASE("Reentrant check: subscribing and unsubscribing the same type from receive() is detected") {
    gViolations = 0;
    struct Mutator : sub0::Subscribe<ReMsg>
    {
        std::optional<PlainSubscriber> child;
        void receive(const ReMsg& m) noexcept override
        {
            if (m.value == 1) child.emplace(); // subscribe during dispatch
            else child.reset();                // unsubscribe during dispatch
        }
    };
    // In snapshot mode, destroying a snapshotted subscriber mid-dispatch is a use-after-free (issue #5)
    if (!cCheckActive)
        return;

    RePublisher pub;
    Mutator mutator;
    pub.send(1);
    CHECK(gViolations == 1);
    pub.send(2);
    CHECK(gViolations == 2);

    // Outside dispatch, table changes are not violations
    mutator.child.emplace();
    mutator.child.reset();
    CHECK(gViolations == 2);
}

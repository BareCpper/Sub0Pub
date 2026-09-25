/** Prototype tests: configuration resolution, per-type policies, scoped domains */
#include "doctest.h"
#include "app_types.hpp"

#include <optional>
#include <type_traits>

namespace {

template<class Data>
struct Sink : sub0x::Subscribe<Data> {
    using sub0x::Subscribe<Data>::Subscribe; // Scoped types construct from a Domain
    int received = 0;
    void receive(const Data&) noexcept override { ++received; }
};

template<class Data>
struct Source : sub0x::Publish<Data> {
    using sub0x::Publish<Data>::Publish;
    void send(const Data& d) noexcept { sub0x::publish(*this, d); }
};

template<class Data, class = void> struct has_filter : std::false_type {};
template<class Data> struct has_filter<Data, std::void_t<decltype(&sub0x::Subscribe<Data>::filter)>> : std::true_type {};

} // namespace

// --- Resolution: each mechanism binds the intended configuration ---
static_assert(sub0x::config_t<Imu>::capacity == 2, "member alias");
static_assert(sub0x::config_t<gps::Fix>::capacity == 3, "ADL declaration");
static_assert(sub0x::config_t<gps::Mode>::capacity == 1, "ADL declaration for an enum");
static_assert(sub0x::config_t<int>::capacity == 5, "traits specialisation");
static_assert(sub0x::config_t<CoreTempC>::capacity == 6, "tagged type configured through its tag");
static_assert(sub0x::config_t<Plain>::capacity == 4, "project default from SUB0X_CONFIG_HEADER");
static_assert(sub0x::config_t<double>::capacity == 4, "unconfigured fundamental type uses the project default");

// Per-type options layer on the project default, not on Builtin
static_assert(sub0x::config_t<Lean>::capacity == 4 && sub0x::config_t<Lean>::dispatch == sub0x::Dispatch::Direct, "");

// Policy-shaped interface: filter() exists only where configured
static_assert(has_filter<Imu>::value, "");
static_assert(!has_filter<gps::Fix>::value, "NoFilter removes the filter() virtual");
static_assert(!has_filter<Lean>::value, "");

// Publishers carry no vtable (no virtual destructor) and only an empty broker handle
static_assert(!std::is_polymorphic_v<sub0x::Publish<Imu>>, "");
static_assert(sizeof(sub0x::Publish<Imu>) == 1, "global-storage publisher is an empty handle");

TEST_CASE("sub0x: per-type capacity is enforced independently") {
    Sink<Imu> a, b, overflow;              // Imu capacity 2
    CHECK(a.isSubscribed());
    CHECK(b.isSubscribed());
    CHECK_FALSE(overflow.isSubscribed());

    Sink<Plain> p[4];                      // Plain capacity 4 (project default)
    for (auto& s : p) CHECK(s.isSubscribed());
    Sink<Plain> plainOverflow;
    CHECK_FALSE(plainOverflow.isSubscribed());

    Source<Imu> src;
    src.send(Imu{});
    CHECK(a.received == 1);
    CHECK(b.received == 1);
    CHECK(overflow.received == 0);
}

TEST_CASE("sub0x: trySubscribe() reclaims a freed slot") {
    std::optional<Sink<Imu>> a{std::in_place};
    Sink<Imu> b, late;
    CHECK_FALSE(late.isSubscribed());
    a.reset();
    CHECK(late.trySubscribe() == sub0x::SubscribeResult::Subscribed);
}

TEST_CASE("sub0x: lean configuration dispatches without context or filter") {
    Sink<Lean> a, b;
    Source<Lean> src;
    src.send(Lean{1});
    CHECK(a.received == 1);
    CHECK(b.received == 1);
}

TEST_CASE("sub0x: cancel() stops dispatch under a context policy") {
    struct Canceller : sub0x::Subscribe<int> {
        void receive(const int&) noexcept override { cancel(); }
    };
    Canceller first;
    Sink<int> second;
    Source<int> src;
    src.send(1);
    CHECK(second.received == 0);
}

TEST_CASE("sub0x: filter() is honoured where configured") {
    struct EvenOnly : sub0x::Subscribe<int> {
        int received = 0;
        bool filter(const int& v) noexcept override { return (v % 2) == 0; }
        void receive(const int&) noexcept override { ++received; }
    };
    EvenOnly s;
    Source<int> src;
    src.send(1);
    src.send(2);
    CHECK(s.received == 1);
}

TEST_CASE("sub0x: scoped domains isolate the same Data type (issue #5)") {
    sub0x::Domain<Session> sessionA, sessionB;
    Sink<Session> subA(sessionA), subB(sessionB);
    Source<Session> pubA(sessionA);
    pubA.send(Session{1});
    CHECK(subA.received == 1);
    CHECK(subB.received == 0);   // no cross-talk
}

// Registry fingerprints effective values, not type names (review finding 2)
namespace {
struct NamedA : sub0x::with<sub0x::Builtin, sub0x::Capacity<3>> {};
struct NamedB : sub0x::with<sub0x::Builtin, sub0x::Capacity<3>> {};
struct NamedC : sub0x::with<sub0x::Builtin, sub0x::Capacity<4>> {};
}
static_assert(sub0x::detail::configFingerprint<NamedA>() == sub0x::detail::configFingerprint<NamedB>(), "same values, different names");
static_assert(sub0x::detail::configFingerprint<NamedA>() != sub0x::detail::configFingerprint<NamedC>(), "different values");

extern int gViolations;

TEST_CASE("sub0x: cancel() from another domain's subscriber does not cancel this domain's dispatch (review finding 3)") {
    sub0x::Domain<Session> a, b;
    Sink<Session> subB(b);

    struct CancelOther : sub0x::Subscribe<Session> {
        using sub0x::Subscribe<Session>::Subscribe;
        sub0x::Subscribe<Session>* other = nullptr;
        int received = 0;
        void receive(const Session&) noexcept override { ++received; other->cancel(); }
    };
    CancelOther first(a);
    first.other = &subB;
    Sink<Session> second(a);

    Source<Session> pubA(a);
    pubA.send(Session{1});
    CHECK(first.received == 1);
    CHECK(second.received == 1); // was skipped before: "A first=1 second=0 B=0"
    CHECK(subB.received == 0);
}

TEST_CASE("sub0x: cancel() still stops its own domain's dispatch") {
    sub0x::Domain<Session> a;
    struct CancelOwn : sub0x::Subscribe<Session> {
        using sub0x::Subscribe<Session>::Subscribe;
        void receive(const Session&) noexcept override { cancel(); }
    };
    CancelOwn first(a);
    Sink<Session> second(a);
    Source<Session> pub(a);
    pub.send(Session{1});
    CHECK(second.received == 0);
}

TEST_CASE("sub0x: DirectChecked reports re-entry into the same domain only") {
    sub0x::Domain<SessionChecked> a, b;
    Sink<SessionChecked> sinkB(b);
    Source<SessionChecked> pubA(a), pubB(b);

    struct Forward : sub0x::Subscribe<SessionChecked> {
        using sub0x::Subscribe<SessionChecked>::Subscribe;
        Source<SessionChecked>* target = nullptr;
        void receive(const SessionChecked& m) noexcept override { if (m.value > 0) target->send(SessionChecked{m.value - 1}); }
    };
    Forward forward(a);

    gViolations = 0;
    forward.target = &pubB;          // A's dispatch publishes into B: independent table, allowed
    pubA.send(SessionChecked{1});
    CHECK(gViolations == 0);
    CHECK(sinkB.received == 1);

    forward.target = &pubA;          // A's dispatch publishes into A: re-entry, reported
    pubA.send(SessionChecked{1});
    CHECK(gViolations == 1);
}

TEST_CASE("sub0x: tagged payloads are distinct channels configured by tag") {
    Sink<CoreTempC> t;
    Source<CoreTempC> src;
    src.send(CoreTempC{42.0f});
    CHECK(t.received == 1);
}

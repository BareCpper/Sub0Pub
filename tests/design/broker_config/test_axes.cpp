/** Prototype tests: the guarantee each policy option claims (docs/design/AXIS_SCORES.md, "Coverage").
 * One behaviour per test, per option, where no other test demonstrated it. */
#include "doctest.h"
#include "sub0x_broker.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

extern int gViolations; // project_config.hpp counts SUB0X_REENTRANT_VIOLATION instead of aborting

namespace {

struct SpinLock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    void lock() noexcept { while (flag.test_and_set(std::memory_order_acquire)) std::this_thread::yield(); }
    void unlock() noexcept { flag.clear(std::memory_order_release); }
};

template<int Id, class... Opts>
struct Msg
{
    int value;
    using sub0_config = sub0x::config<Opts...>;
};

using SnapMsg    = Msg<1, sub0x::Snapshot, sub0x::Capacity<8>>;
using DirectMsg  = Msg<2, sub0x::Direct, sub0x::Capacity<8>>;
using CheckedMsg = Msg<3, sub0x::DirectChecked, sub0x::StaticContext, sub0x::Capacity<8>>;
using StaticMsg  = Msg<4, sub0x::Snapshot, sub0x::StaticContext, sub0x::Capacity<8>>;
using LockedMsg  = Msg<5, sub0x::LockWith<SpinLock>, sub0x::Snapshot, sub0x::ThreadLocalContext, sub0x::Capacity<8>>;
using ChurnMsg   = Msg<6, sub0x::LockWith<SpinLock>, sub0x::Snapshot, sub0x::ThreadLocalContext, sub0x::Capacity<8>>;

template<class Data>
struct Source : sub0x::Publish<Data>
{
    void send(const Data& d) noexcept { sub0x::publish(*this, d); }
};

/// Records deliveries; an optional action runs inside receive() (mutations during dispatch)
template<class Data>
struct Probe : sub0x::Subscribe<Data>
{
    Probe() noexcept { if constexpr (sub0x::detail::cConcurrent<sub0x::config_t<Data>>) this->trySubscribe(); }
    ~Probe() override { this->disconnect(); }
    std::atomic<int> received{0}; // concurrent configurations call a receiver from several publisher threads at once
    std::function<void(const Data&)> action;
    void receive(const Data& d) noexcept override
    {
        received.fetch_add(1, std::memory_order_relaxed);
        if (action)
            action(d);
    }
};

} // namespace

// --- Dispatch = Snapshot: the table may change during a dispatch; the dispatch sees the table as it was ---

TEST_CASE("axes: Snapshot, a subscriber added during dispatch is called from the next publish") {
    Source<SnapMsg> pub;
    std::unique_ptr<Probe<SnapMsg>> late;
    Probe<SnapMsg> first;
    first.action = [&](const SnapMsg&) { if (!late) late = std::make_unique<Probe<SnapMsg>>(); };
    pub.send(SnapMsg{1});
    REQUIRE(late);
    CHECK(late->received == 0);   // not part of the snapshot being delivered
    pub.send(SnapMsg{2});
    CHECK(late->received == 1);
}

TEST_CASE("axes: Snapshot, a later subscriber disconnected during dispatch is not called") {
    Source<SnapMsg> pub;
    Probe<SnapMsg> first;
    Probe<SnapMsg> second;
    first.action = [&](const SnapMsg&) { second.disconnect(); };
    pub.send(SnapMsg{1});
    CHECK(first.received == 1);
    CHECK(second.received == 0);  // removed from this dispatch's snapshot, not only from the table
}

TEST_CASE("axes: Snapshot, re-entrant publish of the same type is delivered (nested)") {
    Source<SnapMsg> pub;
    Probe<SnapMsg> echo;
    echo.action = [&](const SnapMsg& m) { if (m.value > 0) pub.send(SnapMsg{m.value - 1}); };
    pub.send(SnapMsg{2});
    CHECK(echo.received == 3);    // 2, then nested 1, then nested 0
}

// --- Dispatch = Direct: iterates the live table; re-entrant publish is fine, table mutation is not supported ---

TEST_CASE("axes: Direct, re-entrant publish of the same type is delivered (no table mutation)") {
    Source<DirectMsg> pub;
    Probe<DirectMsg> echo;
    echo.action = [&](const DirectMsg& m) { if (m.value > 0) pub.send(DirectMsg{m.value - 1}); };
    pub.send(DirectMsg{2});
    CHECK(echo.received == 3);
}

// --- Dispatch = DirectChecked: any use of the table during its own dispatch is reported ---

TEST_CASE("axes: DirectChecked, re-entrant publish and subscribe during dispatch are both reported") {
    Source<CheckedMsg> pub;
    Probe<CheckedMsg> probe;
    gViolations = 0;
    probe.action = [&](const CheckedMsg& m) { if (m.value > 0) pub.send(CheckedMsg{0}); };
    pub.send(CheckedMsg{1});
    CHECK(gViolations == 1);

    gViolations = 0;
    std::unique_ptr<Probe<CheckedMsg>> late;
    probe.action = [&](const CheckedMsg&) { if (!late) late = std::make_unique<Probe<CheckedMsg>>(); };
    pub.send(CheckedMsg{0});
    CHECK(gViolations == 1);
    probe.action = nullptr;
}

// --- Context = Static: cancel() works without TLS (single-threaded publishers) ---

TEST_CASE("axes: StaticContext, cancel() stops the rest of the current publication only") {
    Source<StaticMsg> pub;
    Probe<StaticMsg> gate;
    Probe<StaticMsg> after;
    gate.action = [&](const StaticMsg& m) { if (m.value == 0) gate.cancel(); };
    pub.send(StaticMsg{0});
    CHECK(after.received == 0);
    pub.send(StaticMsg{1});
    CHECK(after.received == 1);   // the next publication is not affected
}

// --- Lock + ThreadLocalContext: concurrent publishers are isolated from each other's cancel() ---
// (Lock + StaticContext is rejected at compile time: compile_fail/cf_lock_static_context.cpp.)

TEST_CASE("axes: Lock, one publisher thread's cancel() never affects another thread's publication") {
    Probe<LockedMsg> gate;
    std::atomic<int> delivered{0};
    struct Counter : sub0x::Subscribe<LockedMsg>
    {
        explicit Counter(std::atomic<int>& n) noexcept : count(n) { trySubscribe(); }
        ~Counter() override { disconnect(); }
        void receive(const LockedMsg& m) noexcept override { if (m.value >= 0) count.fetch_add(1, std::memory_order_relaxed); }
        std::atomic<int>& count;
    } counter(delivered);
    gate.action = [&](const LockedMsg& m) { if (m.value < 0) gate.cancel(); };

    constexpr int cN = 20000;
    std::thread keeps([&] { Source<LockedMsg> s; for (int i = 0; i < cN; ++i) s.send(LockedMsg{1}); });
    std::thread cancels([&] { Source<LockedMsg> s; for (int i = 0; i < cN; ++i) s.send(LockedMsg{-1}); });
    keeps.join();
    cancels.join();
    CHECK(delivered.load() == cN);
    gate.action = nullptr;
}

// --- Lock: several publishers while subscribers come and go on another thread ---

TEST_CASE("axes: Lock, concurrent publishers with subscribe/unsubscribe churn: stable subscriber loses nothing") {
    std::atomic<int> stableCount{0};
    struct Stable : sub0x::Subscribe<ChurnMsg>
    {
        explicit Stable(std::atomic<int>& n) noexcept : count(n) { trySubscribe(); }
        ~Stable() override { disconnect(); }
        void receive(const ChurnMsg&) noexcept override { count.fetch_add(1, std::memory_order_relaxed); }
        std::atomic<int>& count;
    } stable(stableCount);

    constexpr int cN = 10000;
    std::atomic<bool> publishing{true};
    std::thread churn([&] {
        while (publishing.load(std::memory_order_relaxed))
        {
            Probe<ChurnMsg> transient; // subscribes at construction, disconnects first at destruction
            std::this_thread::yield();
        }
    });
    std::vector<std::thread> publishers;
    for (int t = 0; t < 3; ++t)
        publishers.emplace_back([&] { Source<ChurnMsg> s; for (int i = 0; i < cN; ++i) s.send(ChurnMsg{i}); });
    for (auto& p : publishers)
        p.join();
    publishing.store(false, std::memory_order_relaxed);
    churn.join();
    CHECK(stableCount.load() == 3 * cN);
}

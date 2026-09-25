// Mechanism 2: hazard pointer, no active-dispatch list. Replaces the shared per-publish ActiveDispatch
// list + second lock to unlink (K3) with a fixed, per-table array of hazard slots, CLAIMED (not assigned
// by a growing counter) one per concurrently-publishing thread. A dispatcher publishes the subscriber
// pointer it is about to call into ITS OWN claimed slot (never into memory owned by the subscriber being
// protected -- an earlier version of this file stored the counter *inside* the subscriber object and was
// a genuine use-after-free, caught by TSan: the gap between reading the pointer and writing to the object
// it points to is exactly the race window a real hazard pointer must not have), re-checks the table slot,
// then calls receive(). disconnect() removes the table slot, then spins on the (always-valid, table-owned)
// hazard array -- excluding its own claimed slot, so a subscriber disconnecting itself from inside its own
// receive() cannot wait on itself. See docs/design/spikes/quiescence.md sections 2, 5 and 9 (round 2:
// section 10) for the correctness argument, measurements and the three lifetime probes this design fixes.
#pragma once
#include "qx_common.hpp"
#include <algorithm>
#include <cassert>

#ifndef QX_LOUD_FAILURE
#define QX_LOUD_FAILURE 1 // assert() in addition to refusing + counting; disable for a release-style build
#endif

namespace qx::rc {

template<class Data> class Subscribe;

/// One per (table, concurrently-publishing thread), claimed via ClaimableSlot::claimed (CAS), never
/// assigned by a wrapping counter. `frame` is this thread's nesting stack for THIS Data type: entry i is
/// the hazard published by the i-th nested publish() call still executing on this thread (P2: a receiver
/// that publishes its own type keeps the outer frame's hazard alive in frame[0] while the inner call uses
/// frame[1], instead of one shared cell the inner call would clobber).
template<class Data>
struct HazardSlot : ClaimableSlot
{
    std::array<std::atomic<Subscribe<Data>*>, kMaxNesting> frame{};
    std::atomic<uint32_t> depth{0}; // only the owning thread mutates this; others only read frame[]
};

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    std::atomic<Subscribe<Data>*> entries[kCapacity] = {};
    std::array<HazardSlot<Data>, kMaxReaders> hazard{};
    std::atomic<uint32_t> refusedFull{0};    // report: publish() calls refused, registry had no free slot
    std::atomic<uint32_t> refusedNesting{0}; // report: publish() calls refused, kMaxNesting exceeded
};

template<class Data>
class Broker
{
public:
    static Table<Data>& table() noexcept { static Table<Data> t; return t; }

    static bool trySubscribe(Subscribe<Data>* s) noexcept
    {
        Table<Data>& t = table();
        LockGuard lk(t);
        if (t.count >= kCapacity) return false;
        for (auto& e : t.entries)
            if (e.load(std::memory_order_relaxed) == nullptr)
            {
                e.store(s, std::memory_order_release);
                ++t.count;
                return true;
            }
        return false;
    }

    static void disconnect(Subscribe<Data>* s) noexcept;
    static void publish(const Data& data) noexcept;
};

template<class Data>
class Subscribe
{
public:
    // K5: does not register in the base constructor (another thread could dispatch into a
    // half-constructed derived object). Call activate() at the end of the most-derived constructor.
    Subscribe() noexcept = default;
    virtual ~Subscribe() { disconnect(); }
    void activate() noexcept { subscribed_ = Broker<Data>::trySubscribe(this); }
    void disconnect() noexcept
    {
        if (subscribed_.exchange(false, std::memory_order_relaxed))
            Broker<Data>::disconnect(this);
    }
    virtual void receive(const Data&) noexcept = 0;

private:
    std::atomic<bool> subscribed_{false};
};

template<class Data>
void Broker<Data>::disconnect(Subscribe<Data>* s) noexcept
{
    Table<Data>& t = table();
    {
        LockGuard lk(t);
        for (auto& e : t.entries)
            if (e.load(std::memory_order_relaxed) == s)
            {
                e.store(nullptr, std::memory_order_seq_cst); // publish removal before scanning hazard slots
                --t.count;
            }
    }
#if !QX_MUTATE_SKIP_WAIT
    // P1 fix: never wait on the calling thread's OWN claimed slot -- if this thread is itself mid-callback
    // for s (a self-disconnect), no other thread can ever clear that frame, so waiting on it deadlocks.
    // Mechanism 1's active-dispatch list gets this for free (`a->thread != me`); the per-thread slot model
    // must check it explicitly.
    const std::thread::id me = std::this_thread::get_id();
    for (auto& h : t.hazard)
    {
        if (!h.claimed.load(std::memory_order_acquire) || h.ownerId == me)
            continue;
        // P2 fix: scan every nesting frame, not just one cell -- a nested publish on another thread must
        // not let disconnect() return while an outer frame still names s just because an inner frame does not.
        for (auto& f : h.frame)
        {
            while (f.load(std::memory_order_seq_cst) == s)
            {
                waitIterCounter().fetch_add(1, std::memory_order_relaxed);
                std::this_thread::yield();
            }
        }
    }
#endif
}

template<class Data>
void Broker<Data>::publish(const Data& data) noexcept
{
    Table<Data>& t = table();
    HazardSlot<Data>* const mySlot = myClaimedSlot(t.hazard);
    if (mySlot == nullptr)
    {
        // P3 fix: kMaxReaders concurrently-live publishing threads already claimed every slot. Refuse and
        // report -- never silently alias another thread's slot (that was the P3 bug).
        t.refusedFull.fetch_add(1, std::memory_order_relaxed);
#if QX_LOUD_FAILURE
        assert(false && "qx::rc: hazard slot registry full (kMaxReaders concurrently-publishing threads)");
#endif
        return;
    }
    const uint32_t myDepth = mySlot->depth.fetch_add(1, std::memory_order_relaxed);
    if (myDepth >= kMaxNesting)
    {
        // P2 fix (overflow case): this thread is already kMaxNesting deep in nested same-type publishes.
        t.refusedNesting.fetch_add(1, std::memory_order_relaxed);
#if QX_LOUD_FAILURE
        assert(false && "qx::rc: nested publish() depth exceeded kMaxNesting for this Data type");
#endif
        mySlot->depth.fetch_sub(1, std::memory_order_relaxed);
        return;
    }
    std::atomic<Subscribe<Data>*>& myFrame = mySlot->frame[myDepth];
    // No compaction on disconnect (a slot's index is stable for its subscriber's lifetime), so entries[]
    // can be read directly without the table lock: only the atomics synchronize dispatcher and disconnect().
    for (auto& e : t.entries)
    {
        Subscribe<Data>* const s = e.load(std::memory_order_seq_cst);
        if (s == nullptr) continue;
        myFrame.store(s, std::memory_order_seq_cst); // publish hazard *before* touching s (mutation M1: skip -> races disconnect's scan)
        // Re-check the *same slot*: if disconnect() already nulled it and moved on to scanning hazard
        // slots, our store above (seq_cst) either happened before disconnect's scan (so it sees us and
        // waits) or after (so our re-check below sees the null and we correctly skip s without touching
        // it). Either way s is never dereferenced once disconnect() could have already returned.
        if (e.load(std::memory_order_seq_cst) == s)
            s->receive(data);
        myFrame.store(nullptr, std::memory_order_seq_cst); // mutation M2: skip -> disconnect spins forever on this frame
    }
    mySlot->depth.fetch_sub(1, std::memory_order_relaxed);
}

template<class Data>
inline void publish(const Data& data) noexcept { Broker<Data>::publish(data); }

} // namespace qx::rc

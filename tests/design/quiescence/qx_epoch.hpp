// Mechanism 3: epoch / grace-period reclamation (QSBR-style), fixed reader-slot table, CLAIMED (not
// assigned by a growing counter) one per concurrently-publishing thread -- no allocation, bounded to
// kMaxReaders concurrently-live publishing threads on one table (an embedded-friendly bound; exceeding
// it fails loudly, see myClaimedSlot in qx_common.hpp). A dispatcher publishes the epoch it started in
// before iterating the table; disconnect() removes the entry, bumps the global epoch, and waits until
// every OTHER thread's reader slot that had started strictly before the bump has either gone idle or
// moved on to (or past) the bump epoch. See docs/design/spikes/quiescence.md for the starvation analysis,
// and sections 9-10 for the three lifetime probes this design fixes (self-disconnect, nested publish,
// more publishing threads than kMaxReaders).
#pragma once
#include "qx_common.hpp"
#include <cassert>

#ifndef QX_LOUD_FAILURE
#define QX_LOUD_FAILURE 1
#endif

namespace qx::ep {

template<class Data> class Subscribe;

/// One per (table, concurrently-publishing thread). `depth` counts same-thread NESTING (P2: a receiver
/// that publishes its own type again from inside receive()) instead of a plain bool: only the OUTERMOST
/// (depth 0->1) transition records `epoch`, so a nested call keeps protecting the table at the outer
/// call's (older, more conservative) epoch rather than overwriting it with a newer one that would not
/// cover the outer frame's still-in-flight snapshot.
template<class Data>
struct ReaderSlot : ClaimableSlot
{
    std::atomic<uint32_t> depth{0};  // only the owning thread mutates this; 0 = not reading
    std::atomic<uint64_t> epoch{0};  // valid while depth > 0
};

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    Subscribe<Data>* entries[kCapacity] = {};
    std::atomic<uint64_t> epoch{1};
    std::array<ReaderSlot<Data>, kMaxReaders> readers{};
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
        t.entries[t.count++] = s;
        return true;
    }

    static void disconnect(Subscribe<Data>* s) noexcept
    {
        Table<Data>& t = table();
        {
            LockGuard lk(t);
            for (uint32_t i = 0; i < t.count; ++i)
                if (t.entries[i] == s)
                {
                    std::move(t.entries + i + 1, t.entries + t.count, t.entries + i);
                    --t.count;
                    break;
                }
        }
#if !QX_MUTATE_SKIP_WAIT
        // Grace period: any OTHER thread's slot showing an epoch strictly older than `target` might still
        // be iterating the pre-removal table (mutation M2 analog: skip this wait -> stale pointer used
        // past removal). P1 fix: skip this thread's OWN slot -- a self-disconnect from inside receive()
        // would otherwise wait for its own depth to reach 0, which only the same call could do, deadlock.
        const uint64_t target = t.epoch.fetch_add(1, std::memory_order_acq_rel) + 1;
        const std::thread::id me = std::this_thread::get_id();
        for (auto& r : t.readers)
        {
            if (!r.claimed.load(std::memory_order_acquire) || r.ownerId == me)
                continue;
            while (r.depth.load(std::memory_order_acquire) != 0 && r.epoch.load(std::memory_order_acquire) < target)
            {
                waitIterCounter().fetch_add(1, std::memory_order_relaxed);
                std::this_thread::yield();
            }
        }
#endif
    }

    static void publish(const Data& data) noexcept
    {
        Table<Data>& t = table();
        ReaderSlot<Data>* const my = myClaimedSlot(t.readers);
        if (my == nullptr)
        {
            // P3 fix: kMaxReaders concurrently-live publishing threads already claimed every slot.
            t.refusedFull.fetch_add(1, std::memory_order_relaxed);
#if QX_LOUD_FAILURE
            assert(false && "qx::ep: reader slot registry full (kMaxReaders concurrently-publishing threads)");
#endif
            return;
        }
        const uint32_t myDepth = my->depth.fetch_add(1, std::memory_order_acq_rel);
        if (myDepth >= kMaxNesting)
        {
            t.refusedNesting.fetch_add(1, std::memory_order_relaxed);
#if QX_LOUD_FAILURE
            assert(false && "qx::ep: nested publish() depth exceeded kMaxNesting for this Data type");
#endif
            my->depth.fetch_sub(1, std::memory_order_relaxed);
            return;
        }
        if (myDepth == 0) // only the outermost call (re)publishes the epoch floor -- see ReaderSlot doc
            my->epoch.store(t.epoch.load(std::memory_order_acquire), std::memory_order_release);
        Subscribe<Data>* snapshot[kCapacity];
        uint32_t n;
        {
            LockGuard lk(t);
            n = t.count;
            for (uint32_t i = 0; i < n; ++i) snapshot[i] = t.entries[i];
        }
        for (uint32_t i = 0; i < n; ++i)
            snapshot[i]->receive(data);
        my->depth.fetch_sub(1, std::memory_order_acq_rel);
    }
};

template<class Data>
class Subscribe
{
public:
    // K5: does not register in the base constructor. Call activate() at the end of the most-derived
    // constructor.
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
inline void publish(const Data& data) noexcept { Broker<Data>::publish(data); }

} // namespace qx::ep

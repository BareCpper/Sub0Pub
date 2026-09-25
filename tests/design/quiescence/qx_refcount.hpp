// Mechanism 2: hazard pointer, no active-dispatch list. Replaces the shared per-publish ActiveDispatch
// list + second lock to unlink (K3) with a fixed, per-table array of hazard slots (one per concurrently
// publishing thread, bounded by kMaxReaders -- no allocation). A dispatcher publishes the subscriber
// pointer it is about to call into ITS OWN slot (never into memory owned by the subscriber being
// protected -- an earlier version of this file stored the counter *inside* the subscriber object and
// was a genuine use-after-free, caught by TSan: the gap between reading the pointer and writing to the
// object it points to is exactly the race window a real hazard pointer must not have), re-checks the
// table slot, then calls receive(). disconnect() removes the table slot, then spins on the (always-valid,
// table-owned) hazard array containing no reference to s. See docs/design/spikes/quiescence.md.
#pragma once
#include "qx_common.hpp"
#include <algorithm>

namespace qx::rc {

template<class Data> class Subscribe;

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    std::atomic<Subscribe<Data>*> entries[kCapacity] = {};
    std::array<std::atomic<Subscribe<Data>*>, kMaxReaders> hazard{};
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
    // Wait only while some thread's hazard slot still names s. The hazard array is part of the table
    // (always valid, never freed with the subscriber), so this never touches s's own memory -- unlike the
    // mutated/earlier-broken version, there is no window where we dereference s after it could be freed.
    for (auto& h : t.hazard)
    {
        while (h.load(std::memory_order_seq_cst) == s)
        {
            waitIterCounter().fetch_add(1, std::memory_order_relaxed);
            std::this_thread::yield();
        }
    }
#endif
}

template<class Data>
void Broker<Data>::publish(const Data& data) noexcept
{
    Table<Data>& t = table();
    std::atomic<Subscribe<Data>*>& myHazard = myThreadSlot(t.hazard);
    // No compaction on disconnect (a slot's index is stable for its subscriber's lifetime), so entries[]
    // can be read directly without the table lock: only the atomics synchronize dispatcher and disconnect().
    for (auto& e : t.entries)
    {
        Subscribe<Data>* const s = e.load(std::memory_order_seq_cst);
        if (s == nullptr) continue;
        myHazard.store(s, std::memory_order_seq_cst); // publish hazard *before* touching s (mutation M1: skip -> races disconnect's scan)
        // Re-check the *same slot*: if disconnect() already nulled it and moved on to scanning hazard
        // slots, our store above (seq_cst) either happened before disconnect's scan (so it sees us and
        // waits) or after (so our re-check below sees the null and we correctly skip s without touching
        // it). Either way s is never dereferenced once disconnect() could have already returned.
        if (e.load(std::memory_order_seq_cst) == s)
            s->receive(data);
        myHazard.store(nullptr, std::memory_order_seq_cst); // mutation M2: skip -> disconnect spins forever on this slot
    }
}

template<class Data>
inline void publish(const Data& data) noexcept { Broker<Data>::publish(data); }

} // namespace qx::rc

// Mechanism 3: epoch / grace-period reclamation (QSBR-style), fixed reader-slot table (no allocation,
// bounded to kMaxReaders concurrent publishing threads -- an embedded-friendly bound, cf. hazard-pointer
// lists that grow per thread). A dispatcher publishes the epoch it started in before iterating the table;
// disconnect() removes the entry, bumps the global epoch, and waits until every reader slot that had
// started strictly before the bump has either gone idle or moved on to (or past) the bump epoch. See
// docs/design/spikes/quiescence.md for the starvation analysis.
#pragma once
#include "qx_common.hpp"
#include <array>

namespace qx::ep {

constexpr uint32_t kMaxReaders = 8;

template<class Data> class Subscribe;

struct ReaderSlot
{
    std::atomic<bool> active{false};
    std::atomic<uint64_t> epoch{0};
};

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    Subscribe<Data>* entries[kCapacity] = {};
    std::atomic<uint64_t> epoch{1};
    std::array<ReaderSlot, kMaxReaders> readers{};
};

/// One fixed slot per thread, assigned on first use and never released (bounded thread count is the
/// embedded-friendly trade-off this mechanism makes; a dynamic system would need a free list).
inline ReaderSlot* mySlot(std::array<ReaderSlot, kMaxReaders>& readers) noexcept
{
    static std::atomic<uint32_t> nextSlot{0};
    thread_local ReaderSlot* slot = nullptr;
    thread_local std::array<ReaderSlot, kMaxReaders>* forTable = nullptr;
    if (slot == nullptr || forTable != &readers)
    {
        uint32_t idx = nextSlot.fetch_add(1, std::memory_order_relaxed) % kMaxReaders;
        slot = &readers[idx];
        forTable = &readers;
    }
    return slot;
}

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
        // Grace period: any reader whose slot shows an epoch strictly older than `target` might still be
        // iterating the pre-removal table (mutation M2: skip this wait -> stale pointer used past removal).
        const uint64_t target = t.epoch.fetch_add(1, std::memory_order_acq_rel) + 1;
        for (auto& r : t.readers)
        {
            while (r.active.load(std::memory_order_acquire) && r.epoch.load(std::memory_order_acquire) < target)
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
        ReaderSlot* my = mySlot(t.readers);
        my->epoch.store(t.epoch.load(std::memory_order_acquire), std::memory_order_relaxed);
        my->active.store(true, std::memory_order_release); // publish "I'm reading at this epoch" (mutation M1: skip -> no grace period wait ever sees us)
        Subscribe<Data>* snapshot[kCapacity];
        uint32_t n;
        {
            LockGuard lk(t);
            n = t.count;
            for (uint32_t i = 0; i < n; ++i) snapshot[i] = t.entries[i];
        }
        for (uint32_t i = 0; i < n; ++i)
            snapshot[i]->receive(data);
        my->active.store(false, std::memory_order_release);
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

// Mechanism 1 (baseline): active-dispatch list + seq_cst `current` handshake, reproduced standalone
// from tests/design/broker_config/sub0x_broker.hpp so it can be faced off against the alternatives
// without touching the existing prototype. See docs/design/spikes/quiescence.md, K3/K4/K10.
#pragma once
#include "qx_common.hpp"
#include <algorithm>

namespace qx::hs {

template<class Data> class Subscribe;

template<class Data>
struct ActiveDispatch
{
    std::atomic<Subscribe<Data>*> snapshot[kCapacity];
    uint32_t count = 0;
    std::atomic<Subscribe<Data>*> current{nullptr};
    std::thread::id thread;
    ActiveDispatch* next = nullptr;
};

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    std::atomic<uint32_t> live{0}; // relaxed mirror of count for the empty-table fast path
    Subscribe<Data>* entries[kCapacity] = {};
    ActiveDispatch<Data>* activeHead = nullptr;

    void link(ActiveDispatch<Data>& d) noexcept { d.next = activeHead; activeHead = &d; }
    void unlink(ActiveDispatch<Data>& d) noexcept
    {
        ActiveDispatch<Data>** it = &activeHead;
        while (*it != &d) it = &(*it)->next;
        *it = d.next;
    }
};

/// Broker for one Data type (global table, one instance per Data in this spike).
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
        t.live.store(t.count, std::memory_order_relaxed);
        return true;
    }

    /// After this returns, s->receive() will never run again on any thread.
    static void disconnect(Subscribe<Data>* s) noexcept
    {
        Table<Data>& t = table();
        {
            LockGuard lk(t);
            auto it = std::find(t.entries, t.entries + t.count, s);
            if (it != t.entries + t.count)
            {
                std::move(it + 1, t.entries + t.count, it);
                --t.count;
                t.live.store(t.count, std::memory_order_relaxed);
            }
            // Forget s in every in-flight snapshot (mutation point M1: skip this -> stale snapshot entries)
            for (ActiveDispatch<Data>* a = t.activeHead; a; a = a->next)
                for (uint32_t i = 0; i < a->count; ++i)
                    if (a->snapshot[i].load(std::memory_order_relaxed) == s)
                        a->snapshot[i].store(nullptr, std::memory_order_seq_cst);
        }
#if !QX_MUTATE_SKIP_WAIT
        // Wait only while another thread is mid-callback for s (mutation point M2: skip this wait entirely)
        const std::thread::id me = std::this_thread::get_id();
        for (;;)
        {
            bool busy = false;
            {
                LockGuard lk(t);
                for (ActiveDispatch<Data>* a = t.activeHead; a && !busy; a = a->next)
                {
                    Subscribe<Data>* const cur = a->current.load(std::memory_order_seq_cst);
                    busy = a->thread != me && cur == s;
                }
            }
            if (!busy) break;
            waitIterCounter().fetch_add(1, std::memory_order_relaxed);
            std::this_thread::yield();
        }
#endif
    }

    static void publish(const Data& data) noexcept
    {
        Table<Data>& t = table();
        if (t.live.load(std::memory_order_relaxed) == 0)
            return; // fast path, same in every mechanism: no subscriber to deliver to or protect
        ActiveDispatch<Data> active;
        active.snapshot[0].store(nullptr, std::memory_order_relaxed); // silence unused warnings on empty tables
        {
            LockGuard lk(t);
            active.count = t.count;
            if (active.count == 0)
                return; // nothing to deliver, nothing to protect: no frame to link, no second lock
            for (uint32_t i = 0; i < active.count; ++i)
                active.snapshot[i].store(t.entries[i], std::memory_order_relaxed);
            active.thread = std::this_thread::get_id();
            t.link(active);
        }
        for (uint32_t i = 0; i < active.count; ++i)
        {
            Subscribe<Data>* const s = active.snapshot[i].load(std::memory_order_seq_cst);
            if (s == nullptr) continue;
            active.current.store(s, std::memory_order_seq_cst);
            if (active.snapshot[i].load(std::memory_order_seq_cst) == s) // not disconnected meanwhile
                s->receive(data);
            active.current.store(nullptr, std::memory_order_seq_cst);
        }
        LockGuard lk(t);
        t.unlink(active);
    }
};

template<class Data>
class Subscribe
{
public:
    /// K5: concurrent mechanisms must not register in the base constructor -- another thread could
    /// dispatch into a half-constructed derived object. Call activate() explicitly at the end of the
    /// most-derived constructor (as the sub0x prototype's trySubscribe() contract requires), or use the
    /// CRTP factory in qx_crtp.hpp, which does this for you.
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

} // namespace qx::hs

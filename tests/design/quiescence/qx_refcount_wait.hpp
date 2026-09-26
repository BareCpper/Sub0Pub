// Mechanism 2b: same hazard-pointer scheme as mechanism 2 (qx_refcount.hpp, including the round-2 fixes
// for self-disconnect, nested publish and >kMaxReaders threads), but disconnect() blocks on each hazard
// frame with C++20 std::atomic<T>::wait()/notify_all() instead of yield-spinning, and publish() notifies
// after clearing its frame. Requires C++20 (__cpp_lib_atomic_wait); excluded otherwise. See
// docs/design/spikes/quiescence.md, "C++23 outlook".
#pragma once
#include "qx_common.hpp"
#include <algorithm>
#include <cassert>

#ifndef QX_LOUD_FAILURE
#define QX_LOUD_FAILURE 1
#endif

#if defined(__cpp_lib_atomic_wait)

namespace qx::rcw {

template<class Data> class Subscribe;

template<class Data>
struct HazardSlot : ClaimableSlot
{
    std::array<std::atomic<Subscribe<Data>*>, kMaxNesting> frame{};
    std::atomic<uint32_t> depth{0};
};

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    std::atomic<Subscribe<Data>*> entries[kCapacity] = {};
    std::array<HazardSlot<Data>, kMaxReaders> hazard{};
    std::atomic<uint32_t> refusedFull{0};
    std::atomic<uint32_t> refusedNesting{0};
};

template<class Data>
class Subscribe
{
public:
    // K5: does not register in the base constructor. Call activate() at the end of the most-derived
    // constructor.
    Subscribe() noexcept = default;
    virtual ~Subscribe() { disconnect(); }
    void activate() noexcept;
    void disconnect() noexcept;
    virtual void receive(const Data&) noexcept = 0;

private:
    std::atomic<bool> subscribed_{false};
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

    static void disconnect(Subscribe<Data>* s) noexcept
    {
        Table<Data>& t = table();
        {
            LockGuard lk(t);
            for (auto& e : t.entries)
                if (e.load(std::memory_order_relaxed) == s)
                {
                    e.store(nullptr, std::memory_order_seq_cst);
                    --t.count;
                }
        }
#if !QX_MUTATE_SKIP_WAIT
        // C++20: block (no spin, no allocation) on each OTHER thread's hazard frames until none names s.
        // P1 fix: skip this thread's own claimed slot (see qx_refcount.hpp).
        const std::thread::id me = std::this_thread::get_id();
        for (auto& h : t.hazard)
        {
            if (!h.claimed.load(std::memory_order_acquire) || h.ownerId == me)
                continue;
            for (auto& f : h.frame)
                for (Subscribe<Data>* v; (v = f.load(std::memory_order_seq_cst)) == s;)
                {
                    waitIterCounter().fetch_add(1, std::memory_order_relaxed); // counts wake-ups, not spins
                    f.wait(v, std::memory_order_seq_cst);
                }
        }
#endif
    }

    static void publish(const Data& data) noexcept
    {
        Table<Data>& t = table();
        HazardSlot<Data>* const mySlot = myClaimedSlot(t.hazard);
        if (mySlot == nullptr)
        {
            t.refusedFull.fetch_add(1, std::memory_order_relaxed);
#if QX_LOUD_FAILURE
            assert(false && "qx::rcw: hazard slot registry full");
#endif
            return;
        }
        const uint32_t myDepth = mySlot->depth.fetch_add(1, std::memory_order_relaxed);
        if (myDepth >= kMaxNesting)
        {
            t.refusedNesting.fetch_add(1, std::memory_order_relaxed);
#if QX_LOUD_FAILURE
            assert(false && "qx::rcw: nested publish() depth exceeded kMaxNesting");
#endif
            mySlot->depth.fetch_sub(1, std::memory_order_relaxed);
            return;
        }
        std::atomic<Subscribe<Data>*>& myFrame = mySlot->frame[myDepth];
        for (auto& e : t.entries)
        {
            Subscribe<Data>* const s = e.load(std::memory_order_seq_cst);
            if (s == nullptr) continue;
            myFrame.store(s, std::memory_order_seq_cst); // mutation M1: skip -> races disconnect's wait
            if (e.load(std::memory_order_seq_cst) == s)
                s->receive(data);
            myFrame.store(nullptr, std::memory_order_seq_cst); // mutation M2: skip -> disconnect blocks forever
            myFrame.notify_all();
        }
        mySlot->depth.fetch_sub(1, std::memory_order_relaxed);
    }
};

template<class Data>
void Subscribe<Data>::activate() noexcept { subscribed_ = Broker<Data>::trySubscribe(this); }

template<class Data>
void Subscribe<Data>::disconnect() noexcept
{
    if (subscribed_.exchange(false, std::memory_order_relaxed))
        Broker<Data>::disconnect(this);
}

template<class Data>
inline void publish(const Data& data) noexcept { Broker<Data>::publish(data); }

} // namespace qx::rcw

#endif // __cpp_lib_atomic_wait

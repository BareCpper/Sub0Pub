// Mechanism 2b: same hazard-pointer scheme as mechanism 2 (qx_refcount.hpp), but disconnect() blocks on
// its hazard slot with C++20 std::atomic<T>::wait()/notify_all() instead of yield-spinning, and publish()
// notifies after clearing its slot. Requires C++20 (__cpp_lib_atomic_wait); excluded otherwise. See
// docs/design/spikes/quiescence.md, "C++23 outlook".
#pragma once
#include "qx_common.hpp"
#include <algorithm>

#if defined(__cpp_lib_atomic_wait)

namespace qx::rcw {

template<class Data> class Subscribe;

template<class Data>
struct Table : SpinLock
{
    uint32_t count = 0;
    std::atomic<Subscribe<Data>*> entries[kCapacity] = {};
    std::array<std::atomic<Subscribe<Data>*>, kMaxReaders> hazard{};
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
        // C++20: block (no spin, no allocation) on each hazard slot until it stops naming s.
        for (auto& h : t.hazard)
        {
            for (Subscribe<Data>* v; (v = h.load(std::memory_order_seq_cst)) == s;)
            {
                waitIterCounter().fetch_add(1, std::memory_order_relaxed); // counts wake-ups, not spins
                h.wait(v, std::memory_order_seq_cst);
            }
        }
#endif
    }

    static void publish(const Data& data) noexcept
    {
        Table<Data>& t = table();
        std::atomic<Subscribe<Data>*>& myHazard = myThreadSlot(t.hazard);
        for (auto& e : t.entries)
        {
            Subscribe<Data>* const s = e.load(std::memory_order_seq_cst);
            if (s == nullptr) continue;
            myHazard.store(s, std::memory_order_seq_cst); // mutation M1: skip -> races disconnect's wait
            if (e.load(std::memory_order_seq_cst) == s)
                s->receive(data);
            myHazard.store(nullptr, std::memory_order_seq_cst); // mutation M2: skip -> disconnect blocks forever
            myHazard.notify_all();
        }
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

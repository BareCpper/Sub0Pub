// Mechanism 4: disconnectLater() -- a non-blocking teardown call for use inside a receive(), built on
// mechanism 2's hazard-pointer table. Addresses K4 (blocking disconnect(); two receivers disconnecting
// each other from different threads at the same moment deadlock). disconnectLater() only removes the
// table slot (bounded, lock-protected, no wait) and returns; the *caller* must not actually destroy the
// subscriber until quiescent() is true. This trades a blocking call for a polling contract -- there is no
// mechanism to synchronously know "safe to delete now" without either blocking or polling.
#pragma once
#include "qx_refcount.hpp"

namespace qx::dl {

using qx::rc::Subscribe;
using qx::rc::Table;
using qx::rc::Broker;
using qx::rc::publish; // dispatch is unchanged; only teardown gains a non-blocking entry point

/// Non-blocking: unregisters `s` so no *new* dispatch will call it, then returns immediately.
/// `s` itself may still be mid-callback on another thread; see quiescent().
template<class Data>
inline void disconnectLater(Subscribe<Data>* s) noexcept
{
    Table<Data>& t = Broker<Data>::table();
    LockGuard lk(t);
    for (auto& e : t.entries)
        if (e.load(std::memory_order_relaxed) == s)
        {
            e.store(nullptr, std::memory_order_seq_cst);
            --t.count;
        }
}

/// True once no thread's hazard slot names s any more. The owner polls this (e.g. from an idle task or
/// the next tick) before destroying s; destroying s while this is false is the same use-after-free the
/// other mechanisms prevent by blocking instead.
template<class Data>
inline bool quiescent(const Subscribe<Data>* s) noexcept
{
    Table<Data>& t = Broker<Data>::table();
    for (auto& h : t.hazard)
    {
        if (!h.claimed.load(std::memory_order_acquire))
            continue;
        for (auto& f : h.frame)
            if (f.load(std::memory_order_seq_cst) == s)
                return false;
    }
    return true;
}

/// Convenience: block until quiescent (equivalent to mechanism 2's disconnect()), for call sites outside
/// a receive() that would rather not poll.
template<class Data>
inline void waitQuiescent(const Subscribe<Data>* s) noexcept
{
    while (!quiescent(s))
    {
        waitIterCounter().fetch_add(1, std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

} // namespace qx::dl

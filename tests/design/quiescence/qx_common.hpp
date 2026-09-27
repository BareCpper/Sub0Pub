// Spike (#5): teardown/quiescence face-off. Shared scaffolding for the mechanism variants in this
// directory. See docs/design/spikes/quiescence.md for the write-up.
#pragma once
#include <array>
#include <atomic>
#include <thread>
#include <cstdint>

#ifndef QX_MUTATE_SKIP_WAIT
#define QX_MUTATE_SKIP_WAIT 0
#endif

namespace qx {

/// Trivial spin lock (embedded friendly: no OS wait, pluggable in the real design like sub0x::LockWith<L>)
struct SpinLock
{
    void lock() noexcept
    {
        while (flag_.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();
    }
    void unlock() noexcept { flag_.clear(std::memory_order_release); }
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

struct LockGuard
{
    explicit LockGuard(SpinLock& l) noexcept : l_(l) { l_.lock(); }
    ~LockGuard() { l_.unlock(); }
    SpinLock& l_;
};

constexpr uint32_t kCapacity = 8;

/// Fixed bound on *concurrently claimed* reader/hazard slots a table can track without allocation
/// (embedded-friendly: a dynamic system would need a free list instead). Round 2 fix (quiescence.md
/// section 9, P3): a slot is CLAIMED by CAS and released when the owning thread is done with the table
/// (thread exit, or the thread moves on to a different table), never assigned by an ever-growing counter
/// modulo N -- that let thread 9 silently alias thread 1's slot. Exceeding this many *simultaneously
/// alive* publishing threads on one table now fails loudly (see ClaimedSlot::claim below), not silently.
constexpr uint32_t kMaxReaders = 8;

/// Fixed bound on same-thread NESTED publish depth for the same Data type (round 2 fix, P2: a receiver
/// that publishes its own type from inside receive()). Exceeding it fails loudly, the same as a full
/// registry.
constexpr uint32_t kMaxNesting = 4;

/// This thread's owner token: the address of a thread-local object, unique among live threads and never 0
inline std::uintptr_t myOwnerToken() noexcept
{
    thread_local const char anchor = 0;
    return reinterpret_cast<std::uintptr_t>(&anchor);
}

/// Base for a hazard/reader slot that mechanisms 2 and 3 claim per (table, thread) instead of being
/// handed one by an ever-incrementing counter. Ownership is ONE atomic word: 0 = free, otherwise the owning
/// thread's token (myOwnerToken()). Claim is a CAS from 0 to the token, release stores 0, and a reader's single
/// acquire load answers both "claimed?" and "mine?".
/// (Review fix, 2026-09: the first version kept a `claimed` flag plus a separate non-atomic `ownerId`
/// written AFTER the claiming CAS, so a reader could see claimed == true with a stale owner, and slot reuse
/// raced; TSan reported four races in the quiescence tests. Writing the id before the CAS would instead race
/// between competing claimants.)
struct ClaimableSlot
{
    std::atomic<std::uintptr_t> owner{0};

    bool claimedByOther(std::uintptr_t me) const noexcept
    {
        const std::uintptr_t o = owner.load(std::memory_order_acquire);
        return o != 0 && o != me;
    }
    bool claimed() const noexcept { return owner.load(std::memory_order_acquire) != 0; }
};

/// RAII lease: releases the claimed slot when the owning thread is done with this array, either because
/// the thread exits (thread_local destructor) or because it starts using a different table (a different
/// Data type has its own array, so this is rare in practice but handled for correctness).
template<class Slot, std::size_t N>
struct SlotLease
{
    Slot* slot = nullptr;
    const void* forArray = nullptr;
    ~SlotLease()
    {
        if (slot)
            slot->owner.store(0, std::memory_order_release);
    }
};

/// Claim a free slot in `slots` for the calling thread, caching the claim in thread-local storage so
/// repeated calls are just a load. Returns nullptr when every slot is already claimed by a *different*,
/// still-live thread -- callers MUST treat that as a loud failure (refuse the operation and report it),
/// never fall back to sharing another thread's slot. Allocation-free: claim is a bounded CAS scan.
template<class Slot, std::size_t N>
inline Slot* myClaimedSlot(std::array<Slot, N>& slots) noexcept
{
    thread_local SlotLease<Slot, N> lease;
    if (lease.slot != nullptr && lease.forArray == static_cast<const void*>(&slots))
        return lease.slot;
    if (lease.slot != nullptr) // this thread is switching to a different table's array: release the old one
        lease.slot->owner.store(0, std::memory_order_release);
    lease.slot = nullptr;
    lease.forArray = &slots;
    for (auto& s : slots)
    {
        std::uintptr_t expected = 0;
        if (s.owner.compare_exchange_strong(expected, myOwnerToken(), std::memory_order_acq_rel))
        {
            lease.slot = &s;
            break;
        }
    }
    return lease.slot; // nullptr: registry full
}

/// Global counter of iterations spent spin-waiting in a disconnect() call, for the starvation experiment.
/// Not part of the mechanism itself; a test-only instrument.
inline std::atomic<uint64_t>& waitIterCounter() noexcept
{
    static std::atomic<uint64_t> c{0};
    return c;
}

} // namespace qx

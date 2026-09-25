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

/// Fixed bound on concurrently-publishing threads a table can track without allocation (embedded-friendly
/// trade-off: a dynamic system would need a free list of hazard/reader slots instead).
constexpr uint32_t kMaxReaders = 8;

/// One fixed slot per (array, thread), assigned on first use and never released. Shared by mechanisms 2
/// (hazard pointer) and 3 (epoch): both need a bounded, allocation-free "which threads might be reading
/// right now" registry.
template<class Slot, std::size_t N>
inline Slot& myThreadSlot(std::array<Slot, N>& slots) noexcept
{
    static std::atomic<uint32_t> nextSlot{0};
    thread_local Slot* slot = nullptr;
    thread_local const void* forArray = nullptr;
    if (slot == nullptr || forArray != static_cast<const void*>(&slots))
    {
        const uint32_t idx = nextSlot.fetch_add(1, std::memory_order_relaxed) % static_cast<uint32_t>(N);
        slot = &slots[idx];
        forArray = &slots;
    }
    return *slot;
}

/// Global counter of iterations spent spin-waiting in a disconnect() call, for the starvation experiment.
/// Not part of the mechanism itself; a test-only instrument.
inline std::atomic<uint64_t>& waitIterCounter() noexcept
{
    static std::atomic<uint64_t> c{0};
    return c;
}

} // namespace qx

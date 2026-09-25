/** PROTOTYPE benchmark: sub0x configurations vs the Sub0Pub baseline (docs/PERFORMANCE_BASELINE.md)
 *
 * Each configuration is bound to its own message type, so all of them run in one binary: per-type
 * configuration in action. Built without a project config header, so "Default" is the Builtin
 * configuration (identical policy to sub0pub.hpp's default) and must match the baseline instr/op.
 * Same control conditions as tests/bench/bench_core.cpp: out-of-line publish and lifetime entry points,
 * out-of-line subscribers with more than one implementation.
 */
#define ANKERL_NANOBENCH_IMPLEMENT
#include "sub0x_broker.hpp"

#include "bench_harness.hpp"
#include "bench_system_info.hpp"

#include <atomic>

#if defined(_MSC_VER)
#define BENCH_NOINLINE __declspec(noinline)
#else
#define BENCH_NOINLINE __attribute__((noinline))
#endif

// One message type per configuration under test
struct MsgDefault { int v; };                                                            // Builtin: Snapshot + ThreadLocal + filter
struct MsgDirect { int v; using sub0_config = sub0x::config<sub0x::Direct>; };
struct MsgStatic { int v; using sub0_config = sub0x::config<sub0x::Direct, sub0x::StaticContext>; };
struct MsgNoFilter { int v; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoFilter>; };
struct MsgLean { int v; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };

/// Minimal lock for the concurrent configuration (compare with the baseline's ThreadSafe column)
struct BenchSpinLock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    void lock() noexcept { while (flag.test_and_set(std::memory_order_acquire)) {} }
    void unlock() noexcept { flag.clear(std::memory_order_release); }
};
struct MsgLocked { int v; using sub0_config = sub0x::config<sub0x::LockWith<BenchSpinLock>>; };

namespace bench_types {

template<class Data>
struct NoOpSink : sub0x::Subscribe<Data> {
    void receive(const Data&) noexcept override;
};
template<class Data>
struct CountingSink : sub0x::Subscribe<Data> {
    int count = 0;
    void receive(const Data&) noexcept override;
};
template<class Data> void NoOpSink<Data>::receive(const Data&) noexcept {}
template<class Data> void CountingSink<Data>::receive(const Data&) noexcept { ++count; }

// Explicit instantiation keeps receive() out of line, as in bench_core.cpp
#define SUB0X_BENCH_TYPES(T) template struct NoOpSink<T>; template struct CountingSink<T>;
SUB0X_BENCH_TYPES(MsgDefault)
SUB0X_BENCH_TYPES(MsgDirect)
SUB0X_BENCH_TYPES(MsgStatic)
SUB0X_BENCH_TYPES(MsgNoFilter)
SUB0X_BENCH_TYPES(MsgLean)
SUB0X_BENCH_TYPES(MsgLocked)

template<class Data>
struct Source : sub0x::Publish<Data> {
    BENCH_NOINLINE void send(const Data& d) noexcept { sub0x::publish(*this, d); }
};

template<class Data>
BENCH_NOINLINE void createDestroy() noexcept
{
    NoOpSink<Data> sub;
    if constexpr (sub0x::detail::cConcurrent<sub0x::config_t<Data>>)
        sub.trySubscribe(); // concurrent configurations activate explicitly after construction
    ankerl::nanobench::doNotOptimizeAway(&sub);
}

} // namespace bench_types

namespace {

using namespace bench_types;

template<class Data, std::size_t N>
void publishN(bench::Harness& h, const std::string& name)
{
    Source<Data> pub;
    NoOpSink<Data> subs[N];
    if constexpr (sub0x::detail::cConcurrent<sub0x::config_t<Data>>)
        for (auto& sub : subs)
            sub.trySubscribe();
    h.run(name, [&] { pub.send(Data{42}); });
}

template<class Data>
void runConfig(bench::Harness& h, const std::string& label)
{
    h.title("sub0x " + label);
    {
        Source<Data> pub;
        h.run("publish, 0 subscribers", [&] { pub.send(Data{42}); });
    }
    publishN<Data, 1>(h, "publish, 1 subscriber");
    publishN<Data, 8>(h, "publish, 8 subscribers");
    h.run("create + destroy subscriber", [&] { createDestroy<Data>(); });
}

} // namespace

int main()
{
    printSystemInfo();
    std::cout << "Prototype: sub0x per-type configurations" << std::endl << std::endl;

    bench::Harness h;
    runConfig<MsgDefault>(h, "Default (Snapshot, ThreadLocal, filter)");
    runConfig<MsgDirect>(h, "Direct");
    runConfig<MsgStatic>(h, "Direct + StaticContext (no TLS)");
    runConfig<MsgNoFilter>(h, "Direct + NoFilter");
    runConfig<MsgLean>(h, "Lean (Direct, NoContext, NoFilter)");
    runConfig<MsgLocked>(h, "Locked (Snapshot, spin lock, uncontended)");
    return 0;
}

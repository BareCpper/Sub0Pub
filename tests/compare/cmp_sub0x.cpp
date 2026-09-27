/** v1 vs v2 comparison: the #8 per-type broker prototype (tests/design/broker_config/sub0x_broker.hpp).
 *
 * One message type per configuration, all in one binary. Scenarios a configuration does not offer
 * (filter() without the filter option, cancel() without a publish context) are left out.
 */
#define ANKERL_NANOBENCH_IMPLEMENT
#include "sub0x_broker.hpp"

#include "cmp_common.hpp"

#include <atomic>
#include <string>

struct SpinLock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    void lock() noexcept { while (flag.test_and_set(std::memory_order_acquire)) {} }
    void unlock() noexcept { flag.clear(std::memory_order_release); }
};

struct MsgDefault { int v; }; // Builtin: Snapshot, ThreadLocalContext, filter (v2 header's default policy)
struct MsgSnapStatic { int v; using sub0_config = sub0x::config<sub0x::StaticContext>; };
struct MsgDirect { int v; using sub0_config = sub0x::config<sub0x::Direct>; };
struct MsgLean { int v; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };
struct MsgLocked { int v; using sub0_config = sub0x::config<sub0x::LockWith<SpinLock>>; };

namespace cmp_types {
// Every receiver does the same observable work (one increment), as in cmp_sub0pub.cpp
template<class Data>
struct Counting : sub0x::Subscribe<Data> {
    Counting() noexcept { if constexpr (sub0x::detail::cConcurrent<sub0x::config_t<Data>>) this->trySubscribe(); }
    int count = 0;
    void receive(const Data&) noexcept override;
};
template<class Data>
struct NoOp : sub0x::Subscribe<Data> { // second implementation: receive() stays a real virtual call
    void receive(const Data&) noexcept override;
};
template<class Data>
struct Filtered : Counting<Data> {
    bool filter(const Data& d) noexcept override;
};
template<class Data>
struct Cancelling : Counting<Data> {
    void receive(const Data&) noexcept override;
};
template<class Data> void NoOp<Data>::receive(const Data&) noexcept {}
template<class Data> void Counting<Data>::receive(const Data&) noexcept { ++count; }
template<class Data> bool Filtered<Data>::filter(const Data& d) noexcept { return (d.v & 1) == 0; }
template<class Data> void Cancelling<Data>::receive(const Data&) noexcept { ++this->count; this->cancel(); }

template<class Data>
struct Source : sub0x::Publish<Data> {
    CMP_NOINLINE void send(const Data& d) noexcept { sub0x::publish(*this, d); }
};

template<class Data>
CMP_NOINLINE void createDestroy() noexcept
{
    Counting<Data> sub;
    ankerl::nanobench::doNotOptimizeAway(&sub);
}

template<class Data>
void runConfig(bench::Harness& h, const std::string& label)
{
    using Config = sub0x::config_t<Data>;
    h.title(label);
    {
        Source<Data> pub;
        h.run(cmp::cPublish0, [&] { pub.send(Data{42}); });
    }
    {
        Source<Data> pub;
        Counting<Data> sub;
        h.run(cmp::cPublish1, [&] { pub.send(Data{42}); });
    }
    {
        Source<Data> pub;
        Counting<Data> subs[8];
        (void)subs;
        h.run(cmp::cPublish8, [&] { pub.send(Data{42}); });
    }
    if constexpr (Config::filter)
    {
        Source<Data> pub;
        Filtered<Data> sub;
        h.run(cmp::cFiltered, [&] { pub.send(Data{42}); });
    }
    if constexpr (Config::context != sub0x::Context::None)
    {
        Source<Data> pub;
        Cancelling<Data> first;
        Counting<Data> rest[7];
        (void)rest;
        h.run(cmp::cCancel8, [&] { pub.send(Data{42}); });
    }
    h.run(cmp::cCreateDestroy, [&] { createDestroy<Data>(); });
    NoOp<Data> unused; // keeps a second override live
    (void)unused;
}
} // namespace cmp_types

int main()
{
    using namespace cmp_types;
    bench::Harness h;
    runConfig<MsgDefault>(h, "sub0x Default (Snapshot, ThreadLocal, filter)");
    runConfig<MsgSnapStatic>(h, "sub0x Snapshot + StaticContext (no TLS)");
    runConfig<MsgDirect>(h, "sub0x Direct");
    runConfig<MsgLean>(h, "sub0x Lean (Direct, NoContext, NoFilter)");
    runConfig<MsgLocked>(h, "sub0x Locked (spin lock, uncontended)");
    return 0;
}

/** Spike (#5) benchmark: teardown/quiescence mechanisms face-off, same control conditions and harness
 * as tests/design/broker_config/bench_sub0x.cpp and tests/bench/bench_core.cpp (out-of-line publish and
 * lifetime entry points, out-of-line subscribers). See docs/design/spikes/quiescence.md.
 */
#define ANKERL_NANOBENCH_IMPLEMENT
#include "qx_handshake.hpp"
#include "qx_refcount.hpp"
#include "qx_epoch.hpp"

#include "bench_harness.hpp"
#include "bench_system_info.hpp"

#if defined(_MSC_VER)
#define BENCH_NOINLINE __declspec(noinline)
#else
#define BENCH_NOINLINE __attribute__((noinline))
#endif

struct MsgHandshake { int v; };
struct MsgRefcount { int v; };
struct MsgEpoch { int v; };

// Bind each mechanism's namespace as a template-template-ish adaptor (Subscribe alias + publish())
struct HsNs { template<class D> using Subscribe = qx::hs::Subscribe<D>; template<class D> static void publish(const D& d) noexcept { qx::hs::publish(d); } };
struct RcNs { template<class D> using Subscribe = qx::rc::Subscribe<D>; template<class D> static void publish(const D& d) noexcept { qx::rc::publish(d); } };
struct EpNs { template<class D> using Subscribe = qx::ep::Subscribe<D>; template<class D> static void publish(const D& d) noexcept { qx::ep::publish(d); } };

namespace bench_types {

template<class NS, class Data>
struct NoOpSink : NS::template Subscribe<Data>
{
    NoOpSink() { this->activate(); } // K5: activate only once fully constructed
    ~NoOpSink() override { this->disconnect(); } // lifetime contract: disconnect first, see qx_handshake.hpp
    void receive(const Data&) noexcept override;
};
template<class NS, class Data> void NoOpSink<NS, Data>::receive(const Data&) noexcept {}

template<class NS, class Data>
BENCH_NOINLINE void send(const Data& d) noexcept { NS::publish(d); }

template<class NS, class Data>
BENCH_NOINLINE void createDestroy() noexcept
{
    NoOpSink<NS, Data> sub;
    ankerl::nanobench::doNotOptimizeAway(&sub);
}

template struct NoOpSink<HsNs, MsgHandshake>;
template struct NoOpSink<RcNs, MsgRefcount>;
template struct NoOpSink<EpNs, MsgEpoch>;

} // namespace bench_types

namespace {
using namespace bench_types;

template<class NS, class Data, std::size_t N>
void publishN(bench::Harness& h, const std::string& name)
{
    NoOpSink<NS, Data> subs[N];
    (void)subs;
    h.run(name, [&] { send<NS>(Data{42}); });
}

template<class NS, class Data>
void runConfig(bench::Harness& h, const std::string& label)
{
    h.title("qx " + label);
    h.run("publish, 0 subscribers", [&] { send<NS>(Data{42}); });
    publishN<NS, Data, 1>(h, "publish, 1 subscriber");
    publishN<NS, Data, 8>(h, "publish, 8 subscribers");
    h.run("create + destroy subscriber", [&] { createDestroy<NS, Data>(); });
}

} // namespace

int main()
{
    printSystemInfo();
    std::cout << "Spike #5: teardown/quiescence mechanisms" << std::endl << std::endl;

    bench::Harness h;
    runConfig<HsNs, MsgHandshake>(h, "1 Handshake (active-dispatch list, baseline)");
    runConfig<RcNs, MsgRefcount>(h, "2 Refcount (per-subscriber in-use counter)");
    runConfig<EpNs, MsgEpoch>(h, "3 Epoch (grace period, fixed reader slots)");
    return 0;
}

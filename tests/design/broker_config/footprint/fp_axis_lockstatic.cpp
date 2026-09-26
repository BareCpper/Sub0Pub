/** sub0x footprint: 1 type, 1 publisher, 1 subscriber, 1 publish site (Default, Lock=spin with StaticContext (rejected: concurrent publishers need ThreadLocalContext); docs/design/AXIS_SCORES.md) */
#include "sub0x_broker.hpp"
#include <atomic>
/// Minimal RTOS-style lock: yield() is the RTOS hook the broker calls while quiescing (no std::this_thread needed)
struct SpinLock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    void lock() noexcept { while (flag.test_and_set(std::memory_order_acquire)) {} }
    void unlock() noexcept { flag.clear(std::memory_order_release); }
    static void yield() noexcept {}
};

struct MsgA { int value; using sub0_config = sub0x::config<sub0x::LockWith<SpinLock>, sub0x::StaticContext>; };

struct SinkA : sub0x::Subscribe<MsgA>
{
    SinkA() noexcept { trySubscribe(); } // concurrent configurations activate explicitly after construction
    ~SinkA() override { disconnect(); }
    MsgA last{};
    void receive(const MsgA& d) noexcept override { last = d; }
};
struct SourceA : sub0x::Publish<MsgA> { void send(const MsgA& d) noexcept { sub0x::publish(*this, d); } };

SourceA gSourceA;
SinkA gSinkA;

extern "C" void fp_publish_a(int v) noexcept { gSourceA.send(MsgA{v}); }

extern "C" { char fp_sizeof_Subscribe[sizeof(sub0x::Subscribe<MsgA>)]; char fp_sizeof_Publish[sizeof(sub0x::Publish<MsgA>)]; }

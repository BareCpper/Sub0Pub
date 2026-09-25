// EXPECT: "a Lock requires Snapshot dispatch"
#include "../sub0x_broker.hpp"
struct SpinLock { void lock() noexcept {} void unlock() noexcept {} };
struct Msg { using sub0_config = sub0x::config<sub0x::Direct, sub0x::LockWith<SpinLock>>; };
struct S : sub0x::Subscribe<Msg> { void receive(const Msg&) noexcept override {} };
int main() { S s; }

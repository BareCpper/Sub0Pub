// EXPECT: "a Lock requires Snapshot dispatch"
#include "sub0pub/sub0pub.hpp"
struct SpinLock { void lock() noexcept {} void unlock() noexcept {} };
struct Msg { using sub0_config = sub0::config<sub0::Direct, sub0::LockWith<SpinLock>>; };
struct S : sub0::Subscribe<Msg> { void receive(const Msg&) noexcept override {} };
int main() { S s; }

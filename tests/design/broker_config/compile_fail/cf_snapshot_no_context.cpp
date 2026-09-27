// EXPECT: "Snapshot needs a publish context"
// Without a dispatch frame, a subscriber disconnected (or destroyed) during a Snapshot dispatch cannot be removed from
// that dispatch's snapshot: a receiver deleting a later subscriber left a dangling pointer (ASan: heap-use-after-free).
#include "../sub0x_broker.hpp"
struct Msg { using sub0_config = sub0x::config<sub0x::Snapshot, sub0x::NoContext>; };
struct S : sub0x::Subscribe<Msg> { void receive(const Msg&) noexcept override {} };
int main() { S s; }

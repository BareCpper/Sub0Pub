// EXPECT: "cancel() needs a publish context"
#include "../sub0x_broker.hpp"
struct Msg { using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext>; };
struct S : sub0x::Subscribe<Msg> { void receive(const Msg&) noexcept override { cancel(); } };
int main() { S s; }

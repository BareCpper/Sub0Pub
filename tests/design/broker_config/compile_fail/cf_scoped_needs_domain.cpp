// EXPECT: a Scoped type's subscriber cannot be constructed without a Domain
#include "../sub0x_broker.hpp"
struct Msg { using sub0_config = sub0x::config<sub0x::Scoped>; };
struct S : sub0x::Subscribe<Msg> { void receive(const Msg&) noexcept override {} };
int main() { S s; }

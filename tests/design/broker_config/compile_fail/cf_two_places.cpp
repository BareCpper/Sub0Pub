// EXPECT: "configured in exactly one place"
#include "../sub0x_broker.hpp"
struct Twice { using sub0_config = sub0x::config<sub0x::Capacity<2>>; };
SUB0X_CONFIGURE(Twice, sub0x::Capacity<3>);
struct S : sub0x::Subscribe<Twice> { void receive(const Twice&) noexcept override {} };
int main() { S s; }

// EXPECT: filter() marked override but NoFilter removed it from the interface
#include "../sub0x_broker.hpp"
struct Msg { using sub0_config = sub0x::config<sub0x::NoFilter>; };
struct S : sub0x::Subscribe<Msg> {
    void receive(const Msg&) noexcept override {}
    bool filter(const Msg&) noexcept override { return true; }
};
int main() { S s; }

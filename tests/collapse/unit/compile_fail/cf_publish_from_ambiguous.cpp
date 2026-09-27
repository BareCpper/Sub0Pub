// EXPECT: "Origin must be bound exactly once": two links of one transport type cannot be told apart by type
#include "sandbox/sub0x_static.hpp"
struct Sample { int value; };
struct Radio { void send(const Sample&) noexcept {} };
int main()
{
    Radio a;
    Radio b;
    sub0x::Forward<Radio> linkA(a);
    sub0x::Forward<Radio> linkB(b);
    const auto bus = sub0x::wire(linkA, linkB);
    bus.publishFrom<Radio>(Sample{1});
}

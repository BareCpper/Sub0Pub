// EXPECT: a StaticWiring cannot bind an array element (C++17: a subobject is not a valid template argument), so a
// homogeneous fan-out must be written as distinct named objects (known issue K20)
#include "sandbox/sub0x_static.hpp"
struct Sample { int value; };
struct Controller { void receive(const Sample&) noexcept {} };
Controller controllers[2];
int main()
{
    sub0x::StaticWiring<&controllers[0], &controllers[1]>::publish(Sample{1});
}

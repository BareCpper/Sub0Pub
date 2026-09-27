/** Footprint: 1 type, 1 publisher, 1 receiver, 1 publish site, as pattern B2 StaticWiring (compare fp_1type.cpp) */
#include "sandbox/sub0x_static.hpp"

struct MsgA { int value; };

struct SinkA { MsgA last{}; void receive(const MsgA& d) noexcept { last = d; } };

SinkA gSinkA;
using Bus = sub0x::StaticWiring<&gSinkA>;

extern "C" void fp_publish_a(int v) noexcept { Bus::publish(MsgA{v}); }

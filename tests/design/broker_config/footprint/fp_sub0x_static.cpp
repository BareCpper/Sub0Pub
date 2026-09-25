/** sub0x footprint: 1 type, 1 publisher, 1 subscriber, 1 publish site (Static configuration) */
#include "sub0x_broker.hpp"

struct MsgA { int value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::StaticContext>; };

struct SinkA : sub0x::Subscribe<MsgA> { MsgA last{}; void receive(const MsgA& d) noexcept override { last = d; } };
struct SourceA : sub0x::Publish<MsgA> { void send(const MsgA& d) noexcept { sub0x::publish(*this, d); } };

SourceA gSourceA;
SinkA gSinkA;

extern "C" void fp_publish_a(int v) noexcept { gSourceA.send(MsgA{v}); }

extern "C" { char fp_sizeof_Subscribe[sizeof(sub0x::Subscribe<MsgA>)]; char fp_sizeof_Publish[sizeof(sub0x::Publish<MsgA>)]; }

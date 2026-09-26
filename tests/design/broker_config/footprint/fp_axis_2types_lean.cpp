/** sub0x footprint: 2 types, each with 1 publisher, 1 subscriber, 1 publish site (Lean; the marginal cost of a Data type) */
#include "sub0x_broker.hpp"

struct MsgA { int value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };
struct MsgB { int value; using sub0_config = sub0x::config<sub0x::Direct, sub0x::NoContext, sub0x::NoFilter>; };

struct SinkA : sub0x::Subscribe<MsgA> { MsgA last{}; void receive(const MsgA& d) noexcept override { last = d; } };
struct SourceA : sub0x::Publish<MsgA> { void send(const MsgA& d) noexcept { sub0x::publish(*this, d); } };
struct SinkB : sub0x::Subscribe<MsgB> { MsgB last{}; void receive(const MsgB& d) noexcept override { last = d; } };
struct SourceB : sub0x::Publish<MsgB> { void send(const MsgB& d) noexcept { sub0x::publish(*this, d); } };

SourceA gSourceA;
SinkA gSinkA;
SourceB gSourceB;
SinkB gSinkB;

extern "C" void fp_publish_a(int v) noexcept { gSourceA.send(MsgA{v}); }
extern "C" void fp_publish_b(int v) noexcept { gSourceB.send(MsgB{v}); }

extern "C" { char fp_sizeof_Subscribe[sizeof(sub0x::Subscribe<MsgA>)]; char fp_sizeof_Publish[sizeof(sub0x::Publish<MsgA>)]; }

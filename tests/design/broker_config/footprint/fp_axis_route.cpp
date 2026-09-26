/** sub0x footprint: 1 type, 1 publisher, 1 subscriber, 1 publish site (Default + 1 Route; docs/design/AXIS_SCORES.md) */
#include "sub0x_broker.hpp"

struct MsgA { int value; using sub0_config = sub0x::config<>; };

struct Radio { MsgA last{}; sub0x::SendResult send(const MsgA& d) noexcept { last = d; return sub0x::SendResult::Accepted; } };
struct SinkA : sub0x::Subscribe<MsgA> { MsgA last{}; void receive(const MsgA& d) noexcept override { last = d; } };
struct SourceA : sub0x::Publish<MsgA> { void send(const MsgA& d) noexcept { sub0x::publish(*this, d); } };

SourceA gSourceA;
SinkA gSinkA;
Radio gRadio;
sub0x::Route<MsgA, Radio> gUplink(gRadio);

extern "C" void fp_publish_a(int v) noexcept { gSourceA.send(MsgA{v}); }

extern "C" { char fp_sizeof_Subscribe[sizeof(sub0x::Subscribe<MsgA>)]; char fp_sizeof_Publish[sizeof(sub0x::Publish<MsgA>)]; char fp_sizeof_Route[sizeof(sub0x::Route<MsgA, Radio>)]; }

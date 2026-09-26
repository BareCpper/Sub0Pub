/** sub0x footprint: 1 type, 1 publisher, 1 subscriber, 1 publish site (Default, Storage=Scoped; docs/design/AXIS_SCORES.md) */
#include "sub0x_broker.hpp"

struct MsgA { int value; using sub0_config = sub0x::config<sub0x::Scoped>; };

struct SinkA : sub0x::Subscribe<MsgA> { using Subscribe::Subscribe; MsgA last{}; void receive(const MsgA& d) noexcept override { last = d; } };
struct SourceA : sub0x::Publish<MsgA> { using Publish::Publish; void send(const MsgA& d) noexcept { sub0x::publish(*this, d); } };

sub0x::Domain<MsgA> gDomain;
SourceA gSourceA(gDomain);
SinkA gSinkA(gDomain);

extern "C" void fp_publish_a(int v) noexcept { gSourceA.send(MsgA{v}); }

extern "C" { char fp_sizeof_Subscribe[sizeof(sub0x::Subscribe<MsgA>)]; char fp_sizeof_Publish[sizeof(sub0x::Publish<MsgA>)]; char fp_sizeof_Domain[sizeof(sub0x::Domain<MsgA>)]; }

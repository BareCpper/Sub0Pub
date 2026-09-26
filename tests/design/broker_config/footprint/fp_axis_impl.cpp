/** sub0x footprint: 1 type, 1 publisher, 1 subscriber, 1 publish site (Default, Implementation=SingleSubscriberBroker; docs/design/AXIS_SCORES.md) */
#include "sub0x_broker.hpp"

/// The Implementation<> hook's worked example (test_endpoints.cpp): one subscriber, one pointer of RAM
template<class Data, class Config>
class SingleSubscriberBroker
{
public:
    sub0x::SubscribeResult trySubscribe(sub0x::Subscribe<Data>* s) noexcept
    {
        if (slot() != nullptr)
            return sub0x::SubscribeResult::CapacityExceeded;
        slot() = s;
        return sub0x::SubscribeResult::Subscribed;
    }
    void disconnect(sub0x::Subscribe<Data>* s) noexcept
    {
        if (slot() == s)
            slot() = nullptr;
        sub0x::kit::forgetInOwnDispatches<Data>(&slot(), s);
    }
    void publish(const Data& data, const void* origin, sub0x::PublishReport* report) const noexcept
    {
        sub0x::Subscribe<Data>* snapshot[1] = { slot() };
        sub0x::kit::DispatchScope<Data> scope(&slot(), origin, report, snapshot, 1);
        if (snapshot[0] != nullptr)
            sub0x::kit::deliver(snapshot[0], data);
    }
    void cancel() const noexcept { sub0x::kit::cancel<Data>(&slot()); }
private:
    static sub0x::Subscribe<Data>*& slot() noexcept { static sub0x::Subscribe<Data>* s = nullptr; return s; }
};

struct MsgA { int value; using sub0_config = sub0x::config<sub0x::Implementation<SingleSubscriberBroker>>; };

struct SinkA : sub0x::Subscribe<MsgA> { MsgA last{}; void receive(const MsgA& d) noexcept override { last = d; } };
struct SourceA : sub0x::Publish<MsgA> { void send(const MsgA& d) noexcept { sub0x::publish(*this, d); } };

SourceA gSourceA;
SinkA gSinkA;

extern "C" void fp_publish_a(int v) noexcept { gSourceA.send(MsgA{v}); }

extern "C" { char fp_sizeof_Subscribe[sizeof(sub0x::Subscribe<MsgA>)]; char fp_sizeof_Publish[sizeof(sub0x::Publish<MsgA>)]; }

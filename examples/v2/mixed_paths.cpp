/** Keep the fixed path direct; attach optional runtime observers at one explicit boundary. */
#include "sub0pub/wiring.hpp"
#include "sub0pub/wiring/broker_port.hpp"

struct Sample { unsigned value; };
struct Controller
{
    unsigned total = 0;
    void receive(const Sample& sample) noexcept { total += sample.value; }
};
struct Probe final : sub0::DynamicPort<Sample, 1>::Receiver
{
    unsigned total = 0;
    void receive(const Sample& sample) noexcept override { total += sample.value; }
};

struct SessionSample
{
    unsigned value;
    using sub0_config = sub0::config<sub0::Scoped, sub0::Snapshot>;
};
struct SessionController
{
    unsigned total = 0;
    void receive(const SessionSample& sample) noexcept { total += sample.value; }
};
struct SessionProbe final : sub0::Subscribe<SessionSample>
{
    using Subscribe::Subscribe;
    unsigned count = 0;
    void receive(const SessionSample&) noexcept override
    {
        ++count;
        disconnect(); // Choose BrokerPort + Snapshot when a callback must remove itself.
    }
};

int main()
{
    Controller controller;
    sub0::DynamicPort<Sample, 1> port;
    auto bus = sub0::wire(controller, port);
    bus.publish(Sample{1}); // Empty dynamic side still delivers to controller.
    Probe probe, overflow;
    if (!port.tryAdd(&probe) || port.tryAdd(&overflow)) return 1;
    bus.publish(Sample{2});
    // DynamicPort does not own receivers: remove before destruction, never during delivery.
    // Do not register null/duplicate pointers or access it concurrently.
    port.remove(&probe);
    bus.publish(Sample{3});
    if (controller.total != 6 || probe.total != 2) return 2;

    sub0::Domain<SessionSample> domain;
    SessionController fixed;
    sub0::BrokerPort<SessionSample> brokerPort(domain);
    auto sessionBus = sub0::wire(fixed, brokerPort);
    SessionProbe first(domain), second(domain);
    sessionBus.publish(SessionSample{1}); // Both self-remove safely.
    sessionBus.publish(SessionSample{2}); // Fixed path remains active.
    domain.close();
    sessionBus.publish(SessionSample{3}); // Closing dynamic scope does not close fixed wiring.
    return fixed.total == 6 && first.count == 1 && second.count == 1 ? 0 : 3;
}

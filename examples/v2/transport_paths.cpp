/** Static and runtime transport boundaries: split horizon and explicit acceptance reports. */
#include "sub0pub/wiring.hpp"
#include "sub0pub/broker.hpp"

struct Sample
{
    unsigned value;
    using sub0_config = sub0::config<sub0::Scoped, sub0::ThreadLocalContext>;
};
struct Link
{
    unsigned sent = 0;
    bool full = false;
    sub0::SendResult send(const Sample&) noexcept
    {
        if (full) return sub0::SendResult::Full;
        ++sent; // A real transport copies/serializes the message before returning Accepted.
        return sub0::SendResult::Accepted;
    }
};
struct Local
{
    unsigned count = 0;
    void receive(const Sample&) noexcept { ++count; }
};
struct Observer final : sub0::Subscribe<Sample>
{
    using Subscribe::Subscribe;
    unsigned count = 0;
    void receive(const Sample&) noexcept override { ++count; }
};
struct Source final : sub0::Publish<Sample> { using Publish::Publish; };

Link fixedLink;
Local fixedLocal;
sub0::StaticForward<&fixedLink> fixedForward;
using FixedBus = sub0::StaticWiring<&fixedLocal, &fixedForward>;

int main()
{
    FixedBus::publish(Sample{1});
    FixedBus::publishFrom(fixedLink, Sample{2});
    if (fixedLocal.count != 2 || fixedLink.sent != 1) return 4;
    Link link;
    Local local;
    sub0::Forward<Link> forward(link);
    auto bus = sub0::wire(local, forward);
    bus.publish(Sample{1});
    bus.publishFrom(link, Sample{2}); // Ingress goes local, never back to this link.
    if (local.count != 2 || link.sent != 1) return 1;
    // Forward ignores send() results: handle backpressure in the transport, or use Route below.

    sub0::Domain<Sample> domain;
    Observer observer(domain);
    Source source(domain);
    sub0::Route<Sample, Link> route(domain, link);
    if (!route.isSubscribed()) return 2; // Routes consume subscription capacity too.
    sub0::PublishReport accepted;
    sub0::publish(source, Sample{3}, accepted);
    link.full = true;
    sub0::PublishReport rejected;
    sub0::publish(source, Sample{4}, rejected); // Local delivery survives rejection.
    route.inject(Sample{5}); // No echo to link.
    return observer.count == 3 && link.sent == 2 && accepted.accepted == 1 &&
        rejected.rejected == 1 && rejected.lastRejection == sub0::SendResult::Full ? 0 : 3;
}

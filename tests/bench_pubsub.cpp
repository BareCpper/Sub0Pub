#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"
#include "sub0pub/sub0pub.hpp"

#include <cstdio>

namespace {

struct NoOpSubscriber : sub0::Subscribe<int> {
    void receive(const int&) noexcept override {}
};

struct IntPublisher : sub0::Publish<int> {
    void send(int v) { sub0::publish(this, v); }
};

struct FloatPublisher : sub0::Publish<float> {
    void send(float v) { sub0::publish(this, v); }
};

struct NoOpFloatSub : sub0::Subscribe<float> {
    void receive(const float&) noexcept override {}
};

struct FilteredSubscriber : sub0::Subscribe<int> {
    void receive(const int&) noexcept override {}
    bool filter(const int& v) noexcept override { return (v & 1) == 0; }
};

} // namespace

int main()
{
    ankerl::nanobench::Bench bench;
    bench.warmup(1000).minEpochIterations(1000000);

    // --- Publish throughput ---

    bench.title("Publish throughput");

    {
        IntPublisher pub;
        NoOpSubscriber sub;
        bench.run("1 subscriber", [&] {
            pub.send(42);
        });
    }

    {
        IntPublisher pub;
        NoOpSubscriber subs[4];
        (void)subs;
        bench.run("4 subscribers", [&] {
            pub.send(42);
        });
    }

    {
        IntPublisher pub;
        NoOpSubscriber subs[8];
        (void)subs;
        bench.run("8 subscribers (max default)", [&] {
            pub.send(42);
        });
    }

    // --- Publish with filter ---

    bench.title("Publish with filter");

    {
        IntPublisher pub;
        FilteredSubscriber sub;
        bench.run("1 filtered subscriber (pass)", [&] {
            pub.send(42); // even, passes filter
        });
    }

    {
        IntPublisher pub;
        FilteredSubscriber sub;
        bench.run("1 filtered subscriber (reject)", [&] {
            pub.send(43); // odd, rejected by filter
        });
    }

    // --- Multi-type dispatch ---

    bench.title("Multi-type dispatch");

    {
        struct MultiPub : sub0::Publish<int>, sub0::Publish<float> {
            void sendInt(int v) { sub0::publish(this, v); }
            void sendFloat(float v) { sub0::publish(this, v); }
        };

        MultiPub pub;
        NoOpSubscriber intSub;
        NoOpFloatSub floatSub;

        bench.run("int publish (2-type publisher)", [&] {
            pub.sendInt(42);
        });

        bench.run("float publish (2-type publisher)", [&] {
            pub.sendFloat(3.14f);
        });
    }

    // --- Subscribe/Unsubscribe churn ---

    bench.title("Subscribe/Unsubscribe churn");

    {
        IntPublisher pub;
        bench.run("create + destroy subscriber", [&] {
            NoOpSubscriber sub;
            ankerl::nanobench::doNotOptimizeAway(&sub);
        });
    }

    // --- No subscribers (empty publish) ---

    bench.title("Edge cases");

    {
        IntPublisher pub;
        bench.run("publish with 0 subscribers", [&] {
            pub.send(42);
        });
    }

    return 0;
}

#include "doctest.h"
#include "sub0pub/sub0pub.hpp"

namespace {

struct CancelPublisher : sub0::Publish<int> {
    void send(int value) { sub0::publish(this, value); }
};

struct CancelAfterN : sub0::Subscribe<int> {
    int callCount = 0;
    int cancelAfter;
    CancelAfterN(int n) : cancelAfter(n) {}
    void receive(const int& value) noexcept override {
        ++callCount;
        if (callCount >= cancelAfter)
            cancel();
    }
};

struct Counter : sub0::Subscribe<int> {
    int callCount = 0;
    void receive(const int& value) noexcept override { ++callCount; }
};

} // namespace

TEST_CASE("Cancel stops remaining subscribers") {
    CancelPublisher pub;
    CancelAfterN canceller(1); // Cancel on first receive
    Counter after;             // Should not receive

    pub.send(42);
    CHECK(canceller.callCount == 1);
    CHECK(after.callCount == 0);
}

TEST_CASE("Cancel does not affect subsequent publishes") {
    CancelPublisher pub;
    CancelAfterN canceller(1);
    Counter after;

    pub.send(1); // Cancelled
    CHECK(after.callCount == 0);

    // Reset canceller threshold so it won't cancel again
    canceller.cancelAfter = 999;

    pub.send(2); // Should go through to both
    CHECK(canceller.callCount == 2);
    CHECK(after.callCount == 1);
}

#include "app_types.hpp"

int gMismatches = 0; // counted by SUB0X_CONFIG_MISMATCH (see CMakeLists.txt)

namespace {
struct ImuSource : sub0x::Publish<Imu> {
    void send(const Imu& d) noexcept { sub0x::publish(*this, d); }
};
struct Probe : sub0x::Subscribe<Imu> {
    void receive(const Imu&) noexcept override {}
};
}

void publishImuFromB()
{
    ImuSource src;
    src.send(Imu{});
}

/// Capacity 2: exactly one slot is left if TU a holds one subscriber
int subscribersOfImuSeenByB()
{
    Probe a;
    Probe b;
    return (a.isSubscribed() && !b.isSubscribed()) ? 1 : -1;
}

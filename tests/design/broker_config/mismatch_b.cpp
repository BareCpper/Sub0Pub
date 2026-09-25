#include "sub0x_broker.hpp" // missing SUB0X_CONFIGURE(long, ...): resolves to the project default

namespace {
struct LongSink : sub0x::Subscribe<long> { void receive(const long&) noexcept override {} };
}

void subscribeLongWithoutConfiguration()
{
    LongSink unconfigured;
}

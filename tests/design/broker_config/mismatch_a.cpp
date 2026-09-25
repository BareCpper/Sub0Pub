/** Deliberate ODR hazard: TU a configures `long` via traits, TU b forgot to include that configuration.
 * The debug registry must report it at the second registration. Exit code 0 means it was detected.
 */
#include "sub0x_broker.hpp"

SUB0X_CONFIGURE(long, sub0x::Capacity<3>);

int gMismatches = 0;
void subscribeLongWithoutConfiguration(); // mismatch_b.cpp

namespace {
struct LongSink : sub0x::Subscribe<long> { void receive(const long&) noexcept override {} };
}

int main()
{
    LongSink configured;
    subscribeLongWithoutConfiguration();
    return gMismatches == 1 ? 0 : 1;
}

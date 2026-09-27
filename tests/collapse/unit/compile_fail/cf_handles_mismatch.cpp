// EXPECT: the opt-in K14 guard rejects a receiver whose receive() does not accept the message
#include "sandbox/sub0x_static.hpp"
struct Sample { int value; };
struct Other { int value; };
struct Logger { void receive(const Other&) noexcept {} };
static_assert(sub0x::handles_v<Logger, Sample>, "Logger must receive Sample");
int main() {}

// EXPECT: "Domain<Data> requires a Data type configured with sub0x::Scoped"
#include "../sub0x_broker.hpp"
struct Msg {};
int main() { sub0x::Domain<Msg> d; (void)d; }

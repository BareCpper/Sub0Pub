/** v1 vs v2 comparison: the public header, built against either the v1.0 tag's sub0pub.hpp or the current one.
 *
 * compare_versions.py compiles this file once per header and policy macro set, and passes the column label
 * as CMP_LABEL. Only API common to v1.0 and v2 is used: Subscribe/Publish, virtual receive()/filter(),
 * Subscribe::cancel() and sub0::publish().
 */
#define ANKERL_NANOBENCH_IMPLEMENT
#include <stdexcept> // the v1.0 header uses std::runtime_error without including <stdexcept>
#include "sub0pub/sub0pub.hpp"

#include "cmp_common.hpp"

#ifndef CMP_LABEL
#define CMP_LABEL "sub0pub.hpp"
#endif

namespace cmp_types {
// Every receiver does the same observable work (one increment), so no implementation can drop the call
struct Counting : sub0::Subscribe<int> {
    int count = 0;
    void receive(const int&) noexcept override;
};
struct NoOp : sub0::Subscribe<int> { // second implementation: receive() stays a real virtual call
    void receive(const int&) noexcept override;
};
struct Filtered : sub0::Subscribe<int> {
    int count = 0;
    void receive(const int&) noexcept override;
    bool filter(const int& v) noexcept override;
};
struct Cancelling : sub0::Subscribe<int> {
    int count = 0;
    void receive(const int&) noexcept override;
};
void NoOp::receive(const int&) noexcept {}
void Counting::receive(const int&) noexcept { ++count; }
void Filtered::receive(const int&) noexcept { ++count; }
bool Filtered::filter(const int& v) noexcept { return (v & 1) == 0; }
void Cancelling::receive(const int&) noexcept { ++count; cancel(); }

struct Source : sub0::Publish<int> {
    CMP_NOINLINE void send(int v) noexcept { sub0::publish(*this, v); }
};

CMP_NOINLINE void createDestroy() noexcept
{
    Counting sub;
    ankerl::nanobench::doNotOptimizeAway(&sub);
}
} // namespace cmp_types

int main()
{
    using namespace cmp_types;
    bench::Harness h;
    h.title(CMP_LABEL);
    {
        Source pub;
        h.run(cmp::cPublish0, [&] { pub.send(42); });
    }
    {
        Source pub;
        Counting sub;
        h.run(cmp::cPublish1, [&] { pub.send(42); });
    }
    {
        Source pub;
        Counting subs[8];
        (void)subs;
        h.run(cmp::cPublish8, [&] { pub.send(42); });
    }
    {
        Source pub;
        Filtered sub;
        h.run(cmp::cFiltered, [&] { pub.send(42); });
    }
    {
        Source pub;
        Cancelling first;
        Counting rest[7];
        (void)rest;
        h.run(cmp::cCancel8, [&] { pub.send(42); });
    }
    h.run(cmp::cCreateDestroy, [&] { createDestroy(); });
    NoOp unused; // keeps a second override live, as in tests/bench
    (void)unused;
    return 0;
}

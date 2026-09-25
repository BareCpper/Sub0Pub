/** Spike (#5) lifetime probes, added in review of the quiescence face-off (docs/design/spikes/quiescence.md
 *  section 9). Three scenarios the cross-thread teardown test does not reach, each run in a forked child:
 *    P1  a receiver disconnects itself inside receive()          (safe = returns; else DEADLOCK)
 *    P2  receive() publishes the same type (nested publication), (safe = the other thread's disconnect()
 *        then keeps running while another thread disconnects it       blocks until receive() returns)
 *    P3  more publishing threads than kMaxReaders                 (safe = as P2)
 *  UNSAFE means disconnect() returned while a thread was still executing receive(): a use-after-free
 *  window. Exit status is non-zero if a mechanism marked mustBeSafe is not safe in every probe. Round 2
 *  (quiescence.md section 10) fixed mechanisms 2 and 3 against all three probes; all rows are now
 *  mustBeSafe=true. POSIX only.
 */
#include "qx_handshake.hpp"
#include "qx_refcount.hpp"
#include "qx_epoch.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
using namespace std::chrono_literals;

// P1: receiver disconnects itself inside receive (single thread)
template<template<class> class Sub, class D, class Pub>
int p1() {
    struct S : Sub<D> { S() { this->activate(); } void receive(const D&) noexcept override { this->disconnect(); } ~S() override { this->disconnect(); } };
    S s; Pub::publish(D{0}); return 0; // returning means no deadlock
}

// P2: outer receive publishes same type (nested) then keeps working; thread T disconnects the outer receiver.
template<template<class> class Sub, class D, class Pub>
int p2() {
    static std::atomic<int> inside{0}; static std::atomic<bool> go{false}, disconnected{false};
    struct S : Sub<D> {
        S() { this->activate(); }
        void receive(const D& d) noexcept override {
            if (d.seq != 0) return;             // inner publication: return straight away
            inside = 1;
            Pub::publish(D{1});                  // nested same-type publication (clears a single per-thread hazard)
            go = true;
            auto t0 = std::chrono::steady_clock::now();
            while (!disconnected && std::chrono::steady_clock::now() - t0 < 300ms) std::this_thread::yield();
            inside = disconnected ? 2 : 0;       // 2: disconnect returned while we are still in receive
        }
        ~S() override { this->disconnect(); }
    };
    S* s = new S;
    std::thread t([&]{ while (!go) std::this_thread::yield(); s->disconnect(); disconnected = true; });
    Pub::publish(D{0});
    t.join();
    delete s;
    return inside == 2 ? 1 : 0;
}

// P3: thread X (slot 0) blocks in receive; 7 threads take slots 1..7; thread Y wraps to slot 0, publishes,
// and clears the shared hazard/reader slot; main disconnects while X is still inside receive.
template<template<class> class Sub, class D, class Pub>
int p3() {
    static std::atomic<bool> xInside{false}, release{false};
    static std::thread::id xId;
    struct S : Sub<D> {
        S() { this->activate(); }
        void receive(const D&) noexcept override {
            if (std::this_thread::get_id() != xId) return;
            xInside = true;
            auto t0 = std::chrono::steady_clock::now();
            while (!release && std::chrono::steady_clock::now() - t0 < 500ms) std::this_thread::yield();
            xInside = false;
        }
        ~S() override { this->disconnect(); }
    };
    S* s = new S;
    std::thread x([&]{ xId = std::this_thread::get_id(); Pub::publish(D{0}); });
    while (!xInside) std::this_thread::yield();
    for (int i = 0; i < 7; ++i) std::thread([]{ Pub::publish(D{0}); }).join();
    std::thread([]{ Pub::publish(D{0}); }).join(); // 9th thread: slot index wraps
    std::atomic<bool> done{false};
    std::thread d([&]{ s->disconnect(); done = true; });
    auto t0 = std::chrono::steady_clock::now();
    while (!done && std::chrono::steady_clock::now() - t0 < 200ms) std::this_thread::yield();
    const bool unsafe = done && xInside; // disconnect returned while X is still executing receive
    release = true; x.join(); d.join(); delete s;
    return unsafe ? 1 : 0;
}

template<class F>
const char* runForked(F f) {
    pid_t pid = fork();
    if (pid == 0) { alarm(3); _exit(f()); }
    int st = 0; waitpid(pid, &st, 0);
    if (WIFSIGNALED(st)) return WTERMSIG(st) == SIGALRM ? "DEADLOCK" : "CRASH";
    return WEXITSTATUS(st) ? "UNSAFE" : "safe";
}

template<class D> struct HsP { static void publish(const D& d) { qx::hs::publish(d); } };
template<class D> struct RcP { static void publish(const D& d) { qx::rc::publish(d); } };
template<class D> struct EpP { static void publish(const D& d) { qx::ep::publish(d); } };
template<int N> struct M { int seq; };

static int failures = 0;
static void row(const char* name, bool mustBeSafe, const char* r1, const char* r2, const char* r3)
{
    std::printf("%-10s P1 self-disconnect: %-8s P2 nested publish: %-8s P3 >kMaxReaders: %s\n", name, r1, r2, r3);
    if (mustBeSafe && (std::strcmp(r1, "safe") || std::strcmp(r2, "safe") || std::strcmp(r3, "safe")))
        ++failures;
}
#define ROW(name, NS, P, base, mustBeSafe) \
    row(name, mustBeSafe, \
        runForked([]{ return p1<qx::NS::Subscribe, M<base+1>, P<M<base+1>>>(); }), \
        runForked([]{ return p2<qx::NS::Subscribe, M<base+2>, P<M<base+2>>>(); }), \
        runForked([]{ return p3<qx::NS::Subscribe, M<base+3>, P<M<base+3>>>(); }));
int main() {
    ROW("handshake", hs, HsP, 10, true)
    ROW("hazard", rc, RcP, 20, true) // round 2 fix (quiescence.md section 10): claimed slots + per-thread
                                      // nesting stack + own-thread skip -- all three probes now safe
    ROW("epoch", ep, EpP, 30, true)  // round 2 fix: same three fixes applied to the epoch/reader-slot model
    return failures;
}

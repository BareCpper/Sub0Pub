# Spike: teardown / quiescence mechanism face-off (issue #5)

**Status:** spike, standalone from the existing prototype. **Scope:** the guarantee "after
`disconnect()` returns, `receive()` is never called again on any thread" (section 7 of
[BROKER_CUSTOMISATION.md](../BROKER_CUSTOMISATION.md), known issues K3, K4, K5, K10). Does not touch
`tests/design/broker_config/` or `sub0pub.hpp`. Code: `tests/design/quiescence/`.

> **Review verdict (section 9): the recommendation below is reversed.** Mechanisms 2 (hazard pointer) and 3
> (epoch) fail three lifetime probes that mechanism 1 (handshake, the #8 prototype's design) passes:
> self-disconnect inside `receive()` deadlocks, and a nested same-type publication or a 9th publishing thread
> lets `disconnect()` return while `receive()` is still running. **Mechanism 1 remains the design** until a
> fixed mechanism 2/3 passes the probes and is re-measured with the fixes' cost included.

This spike implements four teardown mechanisms side by side (plus a C++20 outlook variant), measures
each one, and tries to break each one with a deliberate mutation under ASan. It also found and fixed two
real, previously-undocumented lifetime bugs while building the harness — recorded below because they
strengthen the existing "lifetime contracts" text in section 7 rather than replace it.

## 1. Mechanisms

| # | File | Idea |
|---|---|---|
| 1 | `qx_handshake.hpp` | **Baseline**, reproduced standalone from `sub0x_broker.hpp`: each `publish()` links a stack-allocated `ActiveDispatch` (a snapshot copy of the table + a `current` pointer) into the table's active list; `disconnect()` nulls the subscriber out of every snapshot, then waits only while some `ActiveDispatch::current == s`. |
| 2 | `qx_refcount.hpp` | **Hazard pointer**, no active-dispatch list. A fixed, table-owned array of `kMaxReaders` (8) hazard slots, one per concurrently-publishing thread. A dispatcher publishes into *its own* slot the subscriber it is about to call, re-checks the table slot, then calls `receive()`. `disconnect()` removes the table slot, then spins on the (always-valid) hazard array. |
| 3 | `qx_epoch.hpp` | **Epoch / grace period** (QSBR-style). A global per-table epoch counter plus `kMaxReaders` fixed reader slots (`active`, `epoch`). A dispatcher snapshots the epoch before iterating; `disconnect()` bumps the epoch and waits for every reader slot that started strictly before the bump to go idle or move past it. |
| 4 | `qx_deferred.hpp` | **`disconnectLater()`**, built on mechanism 2. Non-blocking: removes the table slot and returns immediately; the caller polls `quiescent(s)` before destroying `s`. Addresses K4. |
| 5 | `qx_crtp.hpp` | **CRTP activation factory**, built on mechanism 1. `Subscriber<Derived, Data>::make(...)` heap-allocates `Derived` and activates it once fully constructed, instead of requiring an explicit `trySubscribe()`/`activate()` call at the end of every derived constructor. Addresses K5, for heap allocation only. |
| 2b | `qx_refcount_wait.hpp` | **C++20 outlook**: mechanism 2 with `disconnect()` blocking on `std::atomic<T>::wait()` instead of yield-spinning, and `publish()` calling `notify_all()`. Built only when `__cpp_lib_atomic_wait` is available; the C++17 baseline never depends on it. |

All five (six, counting 2b) activate explicitly (`activate()`/`trySubscribe()`), not in the base
constructor — see "process notes" below for why that must be strict, not just for K5's `Lock` case.

## 2. Correctness arguments (memory orderings)

**Mechanism 1 (handshake).** Reader: `active.current.store(s, seq_cst)`, then re-load the snapshot slot
(`seq_cst`); if it still equals `s`, call `receive()`. Writer (`disconnect`): null the snapshot slot
(`seq_cst`), then load `current` (`seq_cst`). Both operations on `current`/the snapshot slot are seq_cst,
so there is one global total order over them; whichever side's operation is later in that order observes
the other's effect (store-then-reload on one side matches null-then-load on the other), so `disconnect()`
cannot return while a call it should have prevented is in flight or about to start with the same
snapshot. This is exactly the original prototype's argument (`sub0x_broker.hpp` lines 326-330),
reproduced unchanged.

**Mechanism 2 (hazard pointer).** The critical property, learned the hard way in this spike (see below):
**the hazard/pin state must live outside the memory being protected.** A reader loads `s` from the table
slot, publishes `s` into *its own, table-owned* hazard slot (`seq_cst`), then re-loads the *table slot*
(not `s` itself) to confirm it still holds `s`. `disconnect()` nulls the table slot (`seq_cst`) *before*
scanning the hazard array, and the array it scans is never freed with the subscriber. So: if the reader's
hazard-store precedes `disconnect()`'s scan (in the seq_cst total order over the hazard slot), the scan
sees it and waits. If it follows, the reader's re-check of the (already-nulled) table slot fails and it
never touches `s`. Either way, no operation ever dereferences `s` after `disconnect()` could have
returned — because the thing being raced on (the hazard slot) is never the freed object.

**Mechanism 3 (epoch).** Coarser and intentionally conservative. A reader that starts iterating at global
epoch `E` might have copied `s` into its local snapshot before removal. `disconnect()` removes `s` under
the table lock, then bumps the epoch to `target = E' + 1` and waits for every reader slot showing
`active && epoch < target` to advance. Because the removal happens under the same lock that also
serializes every reader's snapshot copy, any reader whose snapshot could include `s` must have recorded
an epoch `< target` (it read the epoch *before* taking the lock, in program order, and the store is
release/acquire-ordered against the lock itself); `disconnect()` is guaranteed to see that slot as
"stale" and wait for it. The conservative part -- and the source of its worse tail latency (section 5) --
is that a reader can be judged stale (and waited on) even when its particular snapshot did not actually
include `s`, because the scheme only tracks *when* a reader started, not *what* it read.

## 3. Deliberate mutation

Build `QX_MUTATE_SKIP_WAIT=1` (target `Sub0Pub_QxTests_Mutated`) compiles out the final wait/grace-check
in every mechanism (`#if !QX_MUTATE_SKIP_WAIT`). Under this mutation, `disconnect()` still removes the
subscriber from the table but returns immediately, so a subscriber that is mid-callback when destroyed
can be used after free.

## 4. Process notes: two real bugs this spike found (and fixed) while building the harness

1. **Activating in the base constructor is unsafe for every mechanism here, not only a documented
   special case.** The first cut of the `Subscribe` bases activated (`trySubscribe()`) in their own
   constructor, matching the *single-threaded* configurations' contract but not the concurrent ones'. A
   `Guarded` subscriber whose derived constructor sets a member (`counter = new int(0)`) *after* the base
   constructor runs could be registered, and dispatched into, before that member was set -- a real,
   reproducible crash. Fixed by making every mechanism's `Subscribe()` a no-op and requiring an explicit
   `activate()` call at the end of the most-derived constructor (matching K5's documented contract, now
   enforced for all three mechanisms here, not assumed).
2. **A subscriber must call `disconnect()` first in its *own* destructor, even if it has no state to
   protect.** An `Idle` subscriber with no destructor of its own (relying solely on `Subscribe`'s base
   destructor calling `disconnect()`) crashed with "pure virtual method called" under continuous
   publishing. Cause: C++ downgrades an object's vtable to the currently-executing destructor's class
   *at entry* to that destructor, before its body runs. So by the time `Subscribe::~Subscribe()`'s body
   (`disconnect()`) executes, the vtable is already `Subscribe`'s own (`receive()` pure) -- a concurrent
   publisher that read the table slot *before* that point can still call `receive()` during the (however
   short) window before `disconnect()` fences it out. `BROKER_CUSTOMISATION.md` section 7 already
   documents "call `disconnect()` first in the most-derived destructor" for this reason; this spike is,
   as far as this review can tell, the first empirical reproduction of the exact failure it prevents. It
   strengthens the case for a debug-mode check (e.g. a canary flag `Subscribe` sets in its own destructor
   and asserts was already cleared) rather than documentation alone -- filed as a follow-up, not fixed
   here.
3. This is also why mechanism 2's *first draft* stored the hazard state as a plain `inUse_` counter
   **inside** the `Subscribe` object: TSan caught a `delete` racing an atomic `fetch_add` on the same
   address within the first run. The counter-inside-the-object design has the same fatal gap as bug 2:
   the reader must dereference the very memory that `disconnect()` might already be freeing, in the
   window between reading the table slot and touching the counter. Moving the hazard state to a
   fixed, table-owned array (mechanism 2 as shipped, section 2) closes that gap: the reader never
   dereferences `s` to publish its intent.

Both `test_quiescence.cpp` fixture types (`Guarded`, `Idle`) and `bench_qx.cpp`'s `NoOpSink` now follow
the corrected contract (activate last in the constructor, disconnect first in the destructor).

## 5. Evidence

### instr/op (GCC 13, callgrind, `tests/bench/run_baseline.py`-equivalent, `cInstrIterations=10000`)

| Scenario | 1 Handshake | 2 Hazard pointer | 3 Epoch |
|---|---:|---:|---:|
| publish, 0 subscribers | 67 | 77 | **45** |
| publish, 1 subscriber | 95 | 88 | **72** |
| publish, 8 subscribers | 256 | 165 | **118** |
| create + destroy | **108** | 177 | 145 |

For reference, `docs/perf/prototype-sub0x-2026-09.md`'s `Locked` (snapshot + spin lock, mechanism 1's own
family): 90 / 123 / 347, create+destroy 105; baseline `ThreadSafe` (`std::mutex`): 123 / 157 / 333,
create+destroy 218. All three mechanisms here beat both at 8 subscribers; epoch is cheapest everywhere
except create+destroy.

### RAM (GCC 13, `sizeof`, host x86-64; one type)

| | Table (fixed, per type) | Extra per in-flight `publish()` | `Subscribe` (per instance) |
|---|---:|---:|---:|
| 1 Handshake | 80 B | 96 B (stack, one `ActiveDispatch`; not bounded by a fixed constant, but by concurrently-*executing* `publish()` calls, not by thread count) | 16 B |
| 2 Hazard pointer | 136 B (includes 8 hazard slots) | 0 (uses its own fixed slot) | 16 B |
| 3 Epoch | 208 B (includes 8 reader slots + epoch counter) | 0 | 16 B |

Mechanisms 2 and 3 trade a larger, but *fixed and allocation-free*, per-table footprint for no per-call
stack cost; both are capped at `kMaxReaders = 8` concurrently-publishing threads per table (a 9th
concurrent publisher on the same table would collide with another's hazard/reader slot -- not exercised
by any test here, since the benchmarks and tests use at most 4 concurrent publisher threads on one
table). Mechanism 1's list has no such cap.

### Sanitizers (5 runs each, `g++ 13 -O1 -g -fsanitize=...`, full `test_quiescence.cpp`: three
cross-thread "teardown during delivery" tests (2000 stack + 2000 heap `Guarded` subscribers raced
against continuous publishing), the starvation test, and the K4 test)

| | ASan+UBSan | TSan |
|---|---|---|
| Unmutated (`Sub0Pub_QxTests`) | 5/5 clean | 5/5 clean |
| `QX_MUTATE_SKIP_WAIT=1` (`Sub0Pub_QxTests_Mutated`) | **5/5 caught** (heap-use-after-free / SEGV in `receive()`) | not run (ASan is the mutation-detection channel here; TSan reports races, not use-after-free by itself) |

C++20 outlook variant (mechanism 2b, `test_quiescence_cxx20.cpp`, own binary): 5/5 clean under both ASan
and TSan.

This spike's mutation detection was **deterministic across all runs performed** (unlike K10's
"probabilistic, ~4/5" note for the existing `test_endpoints.cpp` `Guarded` test), because of the two
fixes in section 4 plus a wider race window (a 50-iteration dummy spin in the starvation test's
`receive()`, matching the 200-iteration spin already used by the teardown test's `Guarded::receive()`).
Recommend porting the same widening trick back to `test_endpoints.cpp` to make K10's test reliable too.

### Starvation (5 runs, 500 disconnect/reconnect rounds each, 4 continuously-publishing threads racing
one table; `qx::waitIterCounter()`, reset per round, counts spin-wait iterations for mechanisms 1-2 and
wake-ups for 2b; higher = worse tail latency)

| Run | 1 Handshake | 2 Hazard pointer | 3 Epoch |
|---|---:|---:|---:|
| 1 | 0 | 2 | 7 |
| 2 | 4 | 1 | 66 |
| 3 | 1 | 1 | 10 |
| 4 | 2 | 3 | 7 |
| 5 | 0 | 1 | 17,145 |
| (second pass) 1 | 0 | 2 | 134 |
| (second pass) 2 | 1 | 2 | 37 |
| (second pass) 3 | 4 | 16 | 3 |
| (second pass) 4 | 2 | 3 | 183 |
| (second pass) 5 | 1 | 2 | 1,116 |

No run hung or exceeded the test's `CHECK(x < 100000)` bound (one run did hit 17,145 for epoch). All
three mechanisms are bounded in the sense the design requires ("no starvation" -- every observed wait
terminates), but epoch's tail is one to three orders of magnitude worse than handshake's or the hazard
pointer's, consistent with the conservative-by-construction argument in section 2: it sometimes waits out
a reader that could not actually have raced it. Handshake and the hazard pointer both stay in the single
or low double digits.

## 6. Deadlock analysis (K4)

Mechanisms 1-3 all use a **blocking** `disconnect()`: it waits for another thread's in-flight callback to
finish. If subscriber A's `receive()` (running on thread 1) synchronously disconnects B, while B's
`receive()` (running on thread 2) synchronously disconnects A, at the same moment, each blocking call
waits for the very callback that is trying to make it -- a two-cycle wait-for deadlock, identical in
shape to any mutual lock-and-wait bug. This spike does not attempt to trigger that deadlock directly (it
would hang the test binary and, if it ran under CI, the whole suite); the failure mode is a straightforward
consequence of the blocking contract and is already documented as a usage rule in
`BROKER_CUSTOMISATION.md` (K4: "a documented usage rule").

Mechanism 4's `disconnectLater()` cannot deadlock this way *by construction*: it contains no blocking
call, only a lock-protected table update. `test_quiescence.cpp`'s K4 test builds exactly the mutual-
disconnect scenario above (two subscribers, each configured to `disconnectLater()` the other from inside
its own `receive()`, raced from two threads publishing concurrently for 1000 iterations each) and
completes without hanging in every run. The cost: the caller cannot synchronously know "safe to delete
now" -- `quiescent()` must be polled, or `waitQuiescent()` used from a context that is not itself a
receiver (the test does the latter, after the racing threads join).

## 7. C++23 (and beyond) outlook

The maintainer is considering a move to C++23; this spike's headers stay on the project's C++17 baseline
(`cxx_std_17`), with one C++20 variant (mechanism 2b) built only behind `__cpp_lib_atomic_wait` and never
required by the C++17 targets.

- **`std::atomic<T>::wait()`/`notify_*()` (C++20, already usable today under a feature check).**
  Demonstrated here as mechanism 2b: replaces `disconnect()`'s yield-spin with a real blocking wait, and
  `publish()` notifies only when it was the last hazard holder. Sanitizer results were clean (5/5 ASan,
  5/5 TSan); a throughput/latency comparison against the spin variant was not run under callgrind (wait/
  notify's cost is dominated by whether the wait is ever entered, which callgrind's deterministic
  iteration count does not model well -- worth a wall-clock benchmark, not an instruction count, if this
  is pursued further). **Caveat for R5 (Zephyr/embedded):** libstdc++'s implementation needs a futex-like
  primitive; on freestanding/bare-metal targets without one, it is expected to fall back to spinning
  internally (implementation-defined), so this is a strict win only where the standard library has a real
  wait primitive to call into -- not a given on Cortex-M33 without an RTOS underneath it. Worth prototyping
  against the Zephyr toolchain before relying on it there.
- **Hazard pointers and RCU as standard library facilities** (`std::hazard_pointer`,
  `std::rcu_domain`/`std::rcu_synchronize`) are targeted for **C++26** (they are not part of C++23), so
  they are not available yet. Their shape matches mechanism 2 here closely -- adopting the standard
  version later, once C++26 is viable for this project, would replace the hand-rolled fixed hazard-slot
  array with a vetted, likely-unbounded implementation, which could remove the `kMaxReaders` cap noted in
  section 5. This is a "watch for it" item, not an action for the C++23 move itself.
- Nothing else specific to C++23 (`std::expected`, `std::flat_map`, deducing `this`, and so on) changes
  the analysis in this spike; C++20 concepts could tidy the `Broker` concept from
  `BROKER_CUSTOMISATION.md` section 7's proposed answers, but that is a readability improvement, not a
  correctness or performance one.

## 8. Recommendation (as submitted; superseded by section 9)

**Primary: mechanism 2 (hazard pointer, per-thread fixed slots, no active-dispatch list).** It publishes
cheaper than mechanism 1 at 1 and especially 8 subscribers (165 vs 256 instr/op, both well under the
existing `Locked` baseline's 347), keeps a tight, single/low-double-digit disconnect-wait tail (comparable
to mechanism 1, unlike epoch's), and its RAM is fixed and allocation-free (a genuine embedded plus over
mechanism 1's per-call stack node, even though that node is small). It closes the "second lock
acquisition to unlink" half of **K3** outright (no list, no second lock: the writer scans a fixed array
under no lock at all) and reduces, without eliminating, the "seq_cst handshake per subscriber per
publish" half (still one hazard-store, one re-check, and a clear per subscriber per publish -- fewer
total atomics than mechanism 1's snapshot-copy-then-current dance, and empirically cheaper).

**Runner-up: mechanism 3 (epoch).** Cheapest publish path by a clear margin at every subscriber count,
and the mechanism whose story the maintainer may already expect ("epoch-based reclamation"). Recommend it
only where disconnect() is rare on the hot path and its tail latency (up to three orders of magnitude
worse than the other two in this spike's measurements) is acceptable -- it is not a drop-in replacement
for K3 without accepting that trade-off explicitly.

**Layer, don't choose instead:**
- **Mechanism 4 (`disconnectLater`)** on top of mechanism 2, as an *additional* entry point for teardown
  initiated from inside a receiver. It closes **K4** for that specific, common case (a subscriber that
  wants to unsubscribe a peer from within its own callback) without removing the blocking `disconnect()`
  that call sites outside a receiver can keep using.
- **Mechanism 5 (CRTP factory)** as an optional convenience for **K5**, not a replacement for the
  documented `activate()`/`trySubscribe()` contract: it only fully solves the problem for heap-allocated
  subscribers created through `make()`; a stack-allocated subscriber can still forget to activate, since
  C++ has no hook for "after the most-derived constructor" on automatic storage.

**K-items:** K3 -- second-lock-to-unlink closed, seq_cst-per-publish cost reduced (mechanism 2/2b). K4 --
addressed via a non-blocking alternative entry point (mechanism 4), not by removing the blocking
mechanisms' documented deadlock risk. K5 -- reduced for heap allocation (mechanism 5); this spike also
found and documents (section 4) that the base-constructor-activation and destructor-ordering hazards K5
already implies are easy to get wrong even for a stateless subscriber, strengthening the case for a
debug-mode check rather than documentation alone. K10 -- this spike's version of the cross-thread
mutation test was deterministic (5/5) across every run performed; recommend porting its widened race
window back to `test_endpoints.cpp`.

## 9. Review: lifetime probes (added on integration)

The cross-thread teardown test (section 5) only covers one shape of race: a single non-nested publisher
thread and subscribers that never disconnect from inside a callback. Three shapes the existing #8 prototype
already handles were not exercised. `tests/design/quiescence/probe_lifetime.cpp` (ctest `Sub0Pub_QxProbes`,
POSIX only) runs each in a forked child with a timeout:

| Probe | 1 handshake | 2 hazard pointer | 3 epoch |
|---|---|---|---|
| P1 receiver calls `disconnect()` on itself inside `receive()` | safe | **DEADLOCK** | **DEADLOCK** |
| P2 `receive()` publishes the same type (nested), another thread disconnects the outer receiver | safe | **UNSAFE** | **UNSAFE** |
| P3 a 9th publishing thread (more than `kMaxReaders`) | safe | **UNSAFE** | **UNSAFE** |

UNSAFE: `disconnect()` returned while a thread was still executing that subscriber's `receive()`, the
use-after-free the whole guarantee exists to prevent. Mechanism 1 is required to pass every probe (the test
fails otherwise); 2 and 3 are reported. The results are the same under ASan and TSan.

Root causes, all in the per-thread slot model shared by 2 and 3:
- **P1:** `disconnect()` waits for every hazard/reader slot, including the calling thread's own slot, which
  names the subscriber whose `receive()` is making the call. Mechanism 1 recognises the calling thread's own
  dispatch frame and does not wait on it.
- **P2:** one slot per thread holds one hazard (or one epoch). A nested publication overwrites it and
  clears it on exit, so the outer publication is unprotected for the rest of its `receive()`. Nesting needs
  a slot per dispatch frame (a per-thread stack), which is what mechanism 1's frame list already is.
- **P3:** `myThreadSlot()` assigns slots `% kMaxReaders`, so thread 9 silently shares thread 1's slot and
  each clears the other's hazard. A bounded registry has to fail loudly (refuse or assert) when full, or
  fall back to a mechanism that does not need a slot.

**Consequence for section 5's numbers:** the cheaper publish of 2 and 3 is partly the cost of what they
skip: per-frame nesting, own-thread detection, and a checked slot claim. Their instr/op cannot be compared
with mechanism 1's until those fixes are in. **Mechanism 1 (handshake) remains the design**; K3's cost
(seq_cst per subscriber, second lock to unlink) stays a known issue. Mechanism 4 (`disconnectLater`, for
K4) and mechanism 5 (CRTP factory, for K5) are orthogonal to the choice and layer on mechanism 1 unchanged.
A follow-up round fixes 2 and 3, re-runs the probes and re-measures.

Also fixed on integration: `Sub0Pub_QxTests_Cxx20` was `EXCLUDE_FROM_ALL` yet registered with ctest, so a
default build reported it "Not Run" (a ctest failure). It is now built by default.

## 10. Round 2: fixing mechanisms 2 and 3 against the lifetime probes

### 10.1 The fix

Both mechanisms' per-thread slot moved from "assigned by an ever-incrementing counter, one hazard/epoch
value, never released" to a design with three changes, applied identically to both:

1. **Claimed, not assigned (P3).** A slot is taken by `compare_exchange` on a `claimed` flag
   (`qx::ClaimableSlot`, `qx_common.hpp`), not handed out by `nextSlot.fetch_add(1) % N`. It is released
   -- so a later thread can reuse it -- when the owning thread exits (a `thread_local` RAII lease) or
   switches to a different table. **Exceeding `kMaxReaders` *simultaneously live* publishing threads on
   one table now fails loudly**: `publish()` refuses to deliver, increments a `refusedFull` counter on the
   table, and (behind `QX_LOUD_FAILURE`, on by default) asserts. It never silently aliases another
   thread's slot.
2. **A per-thread nesting stack, not one cell (P2).** Each claimed slot gained a fixed
   `frame[kMaxNesting]` array (hazard pointer) or a `depth` counter over a single `epoch` value that is
   only set on the outermost entry (epoch), instead of one hazard/one epoch per thread. A same-thread
   nested `publish()` of the same `Data` type (a receiver publishing its own type) now gets its own frame
   (hazard pointer) or is protected by the still-valid outer epoch (epoch) instead of clobbering the
   thread's only slot. Exceeding `kMaxNesting` (4) fails loudly the same way as a full registry
   (`refusedNesting`).
3. **Own-thread skip in `disconnect()` (P1).** Both mechanisms' wait loop now skips any slot whose
   `ownerId` is the calling thread's own id, mirroring what mechanism 1's `a->thread != me` check already
   did for the active-dispatch list. A subscriber disconnecting itself from inside its own `receive()` no
   longer waits on the very frame that can only be cleared by that same call returning.

`ownerId` is written by the CAS-claiming thread before the `compare_exchange`'s release makes `claimed`
visible, and read by other threads only after they observe `claimed == true` (acquire) -- an ordinary
publish/subscribe pattern, so no extra synchronization was needed for it. The core hazard-pointer and
epoch correctness arguments from section 2 are otherwise unchanged: each frame/epoch cell is still
external to the subscriber object, and the seq_cst store-then-reload (hazard) or lock-serialized
snapshot-then-epoch-bump (epoch) argument still applies per frame.

`qx_refcount_wait.hpp` (mechanism 2b, the C++20 outlook variant) received the same three fixes for
consistency, though the probes only exercise mechanisms 1-3.

### 10.2 Probes: before / after

| Probe | 1 handshake | 2 hazard (before → after) | 3 epoch (before → after) |
|---|---|---|---|
| P1 self-disconnect | safe | DEADLOCK → **safe** | DEADLOCK → **safe** |
| P2 nested publish | safe | UNSAFE → **safe** | UNSAFE → **safe** |
| P3 >`kMaxReaders` threads | safe | UNSAFE → **safe** | UNSAFE → **safe** |

`Sub0Pub_QxProbes` now runs all rows with `mustBeSafe=true` (`probe_lifetime.cpp`), stable across 5 runs
of the release binary and 5 runs each under `ci-asan` and `ci-tsan` (`ctest --preset ci-asan`/`ci-tsan -R
"Sub0Pub_QxTests$|Sub0Pub_QxProbes|Sub0Pub_QxTests_Cxx20"`), all clean. `Sub0Pub_QxTests_Mutated`
(`QX_MUTATE_SKIP_WAIT=1`) still fails 5/5 under `ci-asan` with the same heap-use-after-free/SEGV in
`receive()` as round 1 -- the fix did not touch the `#if !QX_MUTATE_SKIP_WAIT` wait itself, only what it
waits on. `ctest --preset default` passes all 112 registered tests.

### 10.3 Cost of the fix: instr/op (GCC 13, callgrind, same method as section 5)

| Scenario | 1 Handshake | 2 Hazard (before → after) | 3 Epoch (before → after) |
|---|---:|---:|---:|
| publish, 0 subscribers | 67 | 77 → 100 (+23) | 45 → 57 (+12) |
| publish, 1 subscriber | 95 | 88 → 111 (+23) | 72 → 84 (+12) |
| publish, 8 subscribers | 256 | 165 → 188 (+23) | 118 → 130 (+12) |
| create + destroy | 108 | 177 → 198 (+21) | 145 → 163 (+18) |

The extra cost is the claimed-slot lookup (a cached `thread_local` load in the common case, since a slot
is claimed once per thread and reused) plus the nesting-depth increment/decrement (`fetch_add`/`fetch_sub`,
relaxed) on every `publish()` call. After the fix, mechanism 1 is cheapest at 0, 1 subscribers and at
create+destroy; mechanisms 2 and 3 are cheaper only at 8 subscribers (188 and 130 vs 256), and by a smaller
margin than round 1's unfixed numbers showed.

### 10.4 Cost of the fix: RAM (GCC 13, `sizeof`, one type, fixed per-table)

| | Before | After | Why |
|---|---:|---:|---|
| Hazard (mechanism 2) Table | 136 B | 528 B | `kMaxReaders(8) x (claimed + ownerId + kMaxNesting(4) hazard pointers + depth)` replaces 8 plain hazard pointers |
| Epoch (mechanism 3) Table | 208 B | 344 B | `kMaxReaders(8) x (claimed + ownerId + depth + epoch)` replaces 8 `(active, epoch)` pairs |
| Handshake (mechanism 1) Table | 80 B | 80 B (unchanged) | not affected by this round |

Both fixed mechanisms' per-table RAM roughly doubled to more than triple mechanism 1's. Both are still
fixed and allocation-free (no change to that property), and per-`Subscribe` overhead is unchanged (16 B,
all three).

### 10.5 Starvation, re-measured (5 runs, 500 rounds, 4 continuously-publishing threads, same method as
section 5)

| Run | 1 Handshake | 2 Hazard | 3 Epoch |
|---|---:|---:|---:|
| 1 | 0 | 2 | 56 |
| 2 | 0 | 1 | 232 |
| 3 | 1 | 2 | 230 |
| 4 | 0 | 1 | 91 |
| 5 | 0 | 2 | 86 |

Handshake and the hazard pointer are essentially unchanged from round 1 (still single/low-double-digit
spins: the own-thread skip and the extra per-frame scan did not meaningfully change the wait, since this
test has no self-disconnect or nesting to exercise the new code paths' cost). Epoch's tail is, if
anything, still the outlier (56-232 here; round 1 saw one run at 17,145) -- the fix did not change its
conservative-by-construction wait argument (section 2), only fixed the three correctness gaps.

### 10.6 Recommendation, revised

**Mechanism 1 (handshake) is now the clear recommendation, not a fallback.** Before this round, mechanism
2 looked like a plausible win (cheaper at 8 subscribers, fixed RAM) at the cost of three then-undiscovered
correctness gaps. Fixed, mechanism 2 is:
- **more expensive than mechanism 1 at 0 and 1 subscribers** (100 vs 67, 111 vs 95),
- **still cheaper at 8 subscribers** (188 vs 256) but by a smaller margin than round 1 (was 165 vs 256),
- **still more expensive to create+destroy** (198 vs 108, now further behind: was 177 vs 108),
- and now needs **3.3x** mechanism 1's RAM per table for a fixed `kMaxReaders`/`kMaxNesting` bound that
  mechanism 1's linked list does not need at all (section 5's RAM table already noted mechanism 1 has no
  such cap).

Mechanism 3 (epoch) keeps the cheapest publish path at every subscriber count even after the fix (130 at 8
subscribers vs mechanism 1's 256), which is a real number, but its tail latency remains one to three orders
of magnitude worse than the other two, and it costs the most RAM of the three per table (344 B). It is
defensible only for a workload that is publish-heavy, essentially never calls `disconnect()` on the hot
path, and can tolerate an occasional long wait when it does -- a narrower recommendation than round 1's
"cheapest publish path" framing suggested, now that its correctness fix is priced in and its RAM and tail
costs are compared fairly against mechanism 1.

**Mechanisms 4 (`disconnectLater`) and 5 (CRTP factory)** are unaffected in their own right (K4, K5
remain addressed as in section 8) but mechanism 4 is built on mechanism 2's hazard table (`qx_deferred.hpp`
now scans every claimed slot's every frame in `quiescent()`), so it inherits mechanism 2's round-2 RAM and
publish-path cost if adopted together.

**Known issue, recorded:** the round-2 fix trades correctness for a `kMaxReaders x kMaxNesting`-shaped
fixed cost (RAM and per-publish instructions) in both mechanisms 2 and 3, on top of what round 1 already
measured. This is the "loud failure over silent sharing" design explicitly asked for, and it is now
visibly priced rather than a free lunch: any recommendation of mechanism 2 or 3 over mechanism 1 has to be
justified by 8-subscriber (or higher) fan-out being the dominant case for the type in question, not by the
now-outdated round-1 numbers.

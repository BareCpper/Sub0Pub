# #8 policy axes: score matrix and coverage

Every configurable axis of the per-`Data` broker configuration ([BROKER_CUSTOMISATION.md](BROKER_CUSTOMISATION.md)
section 4, prototype `tests/design/broker_config/`) scored option by option, on measured cost and on the guarantees
each option gives. The second half asks whether the use cases behind those scores are exhaustive enough to show each
option's merits and limitations, and records the gaps that were found and closed.

## Method

- **One option at a time.** `bench_axes.cpp` (target `Sub0Pub_Sub0xAxesBench`) changes one option from each of
  two bases, so a delta belongs to that option alone:
  - **Default:** the Builtin configuration (Snapshot, ThreadLocal context, filter, no lock, Global, capacity 8);
  - **Lean:** Direct, NoContext, NoFilter (the cheapest valid configuration).
- **Scenarios:** publish to 0, 1 and 8 subscribers; create + destroy a subscriber; a re-entrant publish (a receiver
  publishes the same type once) where the dispatch policy allows it; and, natively only, 4 threads publishing at once
  for the lock options. Instructions per operation under callgrind (GCC 13 `-O2`), the deterministic bar used by
  [PERFORMANCE_BASELINE.md](../PERFORMANCE_BASELINE.md). Same control conditions as `bench_sub0x.cpp`: out-of-line
  publish and lifetime entry points, virtual receivers with two implementations per type. Raw data:
  [../perf/axes-2026-09.json](../perf/axes-2026-09.json).
- **Footprint:** `tests/footprint/measure_footprint.py` (report: [../perf/axes-footprint-2026-09.md](../perf/axes-footprint-2026-09.md)) builds one object per option (`footprint/fp_axis_*.cpp`: 1 type,
  1 publisher, 1 subscriber, 1 publish site) with `arm-none-eabi-g++ -Os -mcpu=cortex-m33`, and records link-time
  dependencies (TLS is `__aeabi_read_tp`).
- **Guarantees:** each ✓ cites the test that demonstrates it; each ✗ is either a compile error (cited) or a documented
  limitation. `test_axes.cpp` was added for the guarantees nothing tested before.

**Score (0 to 3), per axis, against the cheapest option of the same axis and base:** the geometric mean over
publish 0/1/8 and create+destroy of *option / cheapest*: **3** ≤ ×1.10, **2** ≤ ×1.35, **1** ≤ ×2.0, **0** above.
The footprint score uses Cortex-M33 text + RAM with the same thresholds. Scores rank options *within* an axis; they
are not comparable across axes. Differences below about 6 instructions between loop-based rows can be loop-shape
choices by the compiler (for example GCC unrolls the Scoped publish loop and not the Global one), not the policy.

## Cost and footprint

### Dispatch

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| Snapshot | 31 / 73 / 220 / 72 / 152 | **2** (×1.12) | 27 / 64 / 183 / 72 | **1** (×1.88) | 570 / 66, TLS | **3** |
| Direct | 46 / 61 / 166 / 72 / 128 | **2** (×1.10) | 9 / 33 / 96 / 64 | **3** (×1.00) | 538 / 66, TLS | **3** |
| DirectChecked | 48 / 63 / 168 / 90 / – | **2** (×1.19) | 43 / 54 / 131 / 90 | **1** (×1.97) | 598 / 66, TLS | **3** |

### Context

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| ThreadLocal | 31 / 73 / 220 / 72 / 152 | **3** (×1.00) | 41 / 52 / 129 / 72 | **1** (×1.81) | 570 / 66, TLS | **3** |
| Static | 31 / 73 / 220 / 72 / 152 | **3** (×1.00) | 41 / 52 / 129 / 72 | **1** (×1.81) | 558 / 66 | **3** |
| None | invalid from Default (Snapshot needs a publish context) | – | 9 / 33 / 96 / 64 | **3** (×1.00) | rejected at compile time | – |

### Filter

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| on | 31 / 73 / 220 / 72 / 152 | **2** (×1.13) | 9 / 42 / 133 / 64 | **2** (×1.15) | 570 / 66, TLS | **3** |
| off (NoFilter) | 27 / 63 / 182 / 72 / 132 | **3** (×1.00) | 9 / 33 / 96 / 64 | **3** (×1.00) | 530 / 66, TLS | **3** |

### Lock

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| none | 31 / 73 / 220 / 72 / 152 | **3** (×1.00) | invalid from Lean (a Lock needs Snapshot + ThreadLocalContext) | – | 570 / 66, TLS | **3** |
| spin (LockWith) | 97 / 133 / 378 / 168 / 274 | **0** (×2.19) | invalid from Lean (a Lock needs Snapshot + ThreadLocalContext) | – | 762 / 74, TLS | **2** |
| std::mutex (LockWith) | 223 / 259 / 504 / 499 / 526 | **0** (×4.49) | invalid from Lean (a Lock needs Snapshot + ThreadLocalContext) | – | does not build on newlib (no `std::mutex`) | – |

### Storage

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| Global | 31 / 73 / 220 / 72 / 152 | **3** (×1.00) | 9 / 33 / 96 / 64 | **3** (×1.09) | 570 / 66, TLS | **3** |
| Scoped (Domain) | 37 / 78 / 225 / 88 / 160 | **2** (×1.12) | 19 / 27 / 83 / 77 | **2** (×1.26) | 844 / 132, TLS | **1** |

### Capacity

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| 8 | 31 / 73 / 220 / 72 / 152 | **3** (×1.00) | 9 / 33 / 96 / 64 | **3** (×1.00) | 570 / 66, TLS | **3** |
| 64 | 34 / 78 / 225 / 72 / 162 | **3** (×1.05) | 9 / 33 / 96 / 64 | **3** (×1.00) | 606 / 290, TLS | **1** |

### Implementation

| Option | Default base: pub 0 / pub 1 / pub 8 / create+destroy / re-entrant | score | Lean base: pub 0 / pub 1 / pub 8 / create+destroy | score | Cortex-M33 text / RAM (B), TLS | score |
|---|---|---:|---|---:|---|---:|
| library broker | 31 / 73 / 220 / 72 / 152 | **2** (×1.30) | not measured | – | 570 / 66, TLS | **1** |
| SingleSubscriberBroker (Implementation<>) | 35 / 47 / – / 39 / 100 | **3** (×1.04) | not measured | – | 382 / 34, TLS | **3** |

### Route (a transport endpoint bound to the type)

| | Default base: publish, 1 route | publish, 1 subscriber + 1 route | vs 1 subscriber alone | Cortex-M33 text / RAM (B) |
|---|---:|---:|---:|---|
| 1 `Route<Data, Transport>` | 81 | 113 | 73 (a route costs about what one subscriber costs, +8 for split-horizon lookup) | 758 / 94, TLS (+188 B text, +28 B RAM over Default) |

### Cost of a second `Data` type (per-type code)

| Cortex-M33 | 1 type | 2 types | marginal type |
|---|---:|---:|---:|
| Default | 570 / 66 | 1128 / 119 | **+558 B text, +53 B RAM** |
| Lean | 394 / 62 | 776 / 111 | **+382 B text, +49 B RAM** |

Nothing is shared between `Data` types: each type instantiates its own table, dispatch loop and handle code. See
known issue K17.

### Contention (native only; 4 threads publishing to 1 subscriber, wall time per publish)

| Lock | ns/publish |
|---|---:|
| spin (`LockWith<SpinLock>`, yield when busy) | 621 |
| `std::mutex` | 164 |

The spin lock is 2× cheaper uncontended (128 vs 256 instr/op for 1 subscriber) and 3.8× slower under
contention on this host. Neither number predicts an RTOS lock (Zephyr `k_spinlock` masks interrupts); that needs
target measurement.

## Guarantees

| Axis / option | Re-entrant publish (same type, same thread) | Table changed during its own dispatch (same thread) | Concurrent publishers | Teardown during delivery from another thread | `cancel()` | Other | Evidence |
|---|---|---|---|---|---|---|---|
| **Dispatch = Snapshot** | ✓ delivered nested | ✓ a subscriber added is called from the next publish; one removed (or destroyed) is not called again | with a Lock | with a Lock | with a context | requires a publish context (compile error otherwise: `cf_snapshot_no_context`) | `test_axes`: 4 Snapshot tests incl. destruction under ASan; `test_endpoints`: same-thread teardown |
| **Dispatch = Direct** | ✓ delivered nested | ✗ unsupported and undetected | ✗ (a Lock requires Snapshot: `cf_lock_direct`) | ✗ | with a context | fastest dispatch | `test_axes`: Direct re-entrant test |
| **Dispatch = DirectChecked** | ✗ reported | ✗ reported | ✗ | ✗ | ✓ (context required) | needs a context (compile error otherwise) | `test_axes`, `test_binding` (per-domain) |
| **Context = ThreadLocal** | – | – | ✓ each thread's `cancel()` and dispatch frames are its own | ✓ | ✓ | needs TLS (`__aeabi_read_tp` on Cortex-M) | `test_axes`: cancel isolation under a Lock; `test_binding` cancel |
| **Context = Static** | – | – | ✗ **compile error with a Lock** (new: `cf_lock_static_context`) | ✗ | ✓ single-threaded | no TLS | `test_axes`: StaticContext cancel |
| **Context = None** | – | – | ✗ (a Lock requires a context) | ✗ | ✗ compile error (`cf_cancel_no_context`) | Direct dispatch only (no Snapshot, no DirectChecked), no routes, no reports | compile errors |
| **Filter = on** | – | – | – | – | – | `filter()` skips a subscriber per message; a `filter()` that disconnects or destroys its subscriber, or closes its domain, prevents `receive()` | `test_binding` filter; `test_axes`: 3 filter lifetime tests (ASan) |
| **Filter = off** | – | – | – | – | – | overriding `filter()` is a compile error (`cf_filter_disabled`) | compile error |
| **Lock = none** | – | – | ✗ not supported | ✗ | – | subscribers activate at construction | – |
| **Lock = `LockWith<L>`** | ✓ | ✓ | ✓ publishers and churn on other threads lose nothing | ✓ `disconnect()` waits only for a callback running elsewhere | ✓ per thread | explicit `trySubscribe()` after construction (K5); blocking disconnect (K4) | `test_axes`: churn and cancel isolation; `test_endpoints` cross-thread teardown; TSan in CI |
| **Storage = Global** | – | – | – | – | – | one table per type; shared across TUs | `test_multi_tu_*`, mismatch detection |
| **Storage = Scoped** | – | – | – | ✓ `Domain::close()` quiesces | ✓ scoped to its domain | sessions isolated; needs the library broker (K8) | `test_binding`, `test_endpoints` |
| **Capacity = N** | – | – | – | – | – | full table reported (`SubscribeResult::CapacityExceeded`), freed slot reclaimed | `test_binding` |
| **Implementation<>** | per implementation | per implementation | per implementation | per implementation | via `kit` | own capacity rule; Global storage only (K8) | `test_endpoints` |
| **Route** | – | – | with a Lock | ✓ | – | egress, ingress with split horizon, rejection reports; needs a context | `test_endpoints` |

## Reading the matrix

- **Pay for what you use holds per option.** Every option that adds a guarantee has a measured price, and every
  option that removes one scores 3 on cost: Direct, Context None, Filter off, Global, the smallest capacity.
- **The expensive options are the guarantees for threads and time:** a Lock (×2.2 uncontended with a spin lock, ×4.5
  with `std::mutex`) and a publish context (×1.8 from Lean, because it pushes a dispatch frame). Snapshot costs ×1.9
  from Lean, including the publish context it requires, and buys safe table changes during dispatch.
- **Capacity costs RAM only** (64 slots: +224 B on Cortex-M33, publish unchanged): size it per type.
- **Scoped storage** costs +5 to +13 instructions on an empty publish and on create/destroy and, on Cortex-M33, +274 B text and +66 B RAM for the domain
  machinery.
- **A custom broker through `Implementation<>`** is the cheapest way to express a special case (one subscriber:
  create+destroy 39 vs 72, 382 B vs 570 B).

## Are the use cases exhaustive? (coverage review)

Each option's merits and limitations need a use case that exercises them. Before this review:

| Use case | Demonstrated before | Now |
|---|---|---|
| publish to 0 / 1 / 8 subscribers, create + destroy | 6 whole configurations (several options changed at once) | every option alone, from two bases (`bench_axes`) |
| re-entrant publish of the same type | no benchmark; DirectChecked reporting only | cost per dispatch/context option; delivery tested for Snapshot and Direct, reporting for DirectChecked |
| table changed during dispatch (same thread) | teardown during delivery only | subscribe and disconnect during dispatch tested for Snapshot, reported for DirectChecked |
| concurrent publishers | one publisher thread (cross-thread teardown test) | 3 publishers with subscribe/unsubscribe churn; per-thread cancel isolation (TSan) |
| lock contention | none (uncontended only) | spin vs `std::mutex`, 4 threads |
| `DirectChecked`, `ThreadLocal` alone, Scoped, Capacity, `Implementation<>`, Route | functional tests only, no cost | cost and footprint for each |
| footprint per option (Cortex-M33) | 4 configurations | every option, plus the cost of a second type |
| invalid option combinations | 6 compile-fail checks | 8 (Lock + StaticContext, Snapshot + NoContext added) |
| lifetime during dispatch: a receiver or `filter()` destroying subscribers, closing the domain | receiver disconnect only | destruction and domain close, from `receive()` and `filter()`, under ASan |

**Defect found by the review:** a Lock with `StaticContext` compiled, but `StaticContext` keeps one frame stack for the
process. With concurrent publishers, one thread's `cancel()` stopped another thread's publication and the frame list
raced. A reproduction lost 3 to 10 of 200 000 deliveries per release run, and TSan reported data races. **Fixed:** a
Lock now requires `ThreadLocalContext` (compile error, `cf_lock_static_context`); `test_axes` shows the
ThreadLocal combination is correct. The price (a Lock needs TLS) is known issue K16.

**Defects found by the independent review (2026-09), fixed:**
- **Snapshot + NoContext use-after-free.** `forgetInOwnDispatches` finds a dispatch's snapshot through its frame, and
  `NoContext` has no frame: a receiver destroying a later subscriber left a dangling pointer in the snapshot (ASan:
  heap-use-after-free). Snapshot's guarantee depended on the Context axis without saying so. **Fixed:** Snapshot
  requires a publish context (compile error, `cf_snapshot_no_context`); the cheapest sound Snapshot is `+StaticContext`.
  From the Default base, Context = None is therefore invalid, and the Lean-base Snapshot row includes StaticContext.
- **Delivery after a disconnect inside `filter()`.** Delivery called `filter()` then `receive()` without looking
  again, so a `filter()` that disconnected its subscriber and returned true still got `receive()`, and one that destroyed
  it made a call on a freed object. **Fixed:** `kit::deliverAt` re-reads the dispatch-owned slot (never the subscriber)
  after `filter()`; Direct dispatch skips the null check its live entries never need. Measured cost: none where `filter()`
  is disabled or devirtualised (Lean 9/33/96 and Direct 61/166 unchanged; Snapshot 73/220 from 74/228). With a real
  `filter()` override, which this benchmark lacks, it is 2 instructions per delivered subscriber on gcc and clang
  (v1/v2 comparison, [../perf/compare-v1-v2-2026-09.md](../perf/compare-v1-v2-2026-09.md)). `test_axes` covers disconnect, self-destruction and domain
  close from `filter()`, and ASan catches the self-destruction case when the re-check is removed.

**Still not covered (open):**
- Publishing from an interrupt (ISR) with `StaticContext`, and a deferred-dispatch (queue) policy: the prototype has no
  ISR harness and no queue axis (planned axis, section 4).
- Contention and lock cost on the target (Zephyr `k_spinlock`, nRF54): host numbers only.
- Priority inversion and bounded wait under an RTOS scheduler (K4).
- MSVC evidence (COLLAPSE_EVIDENCE.md plan).
- The static-subscriber axis (pattern B, #9) selected per `Data` type through this configuration: Phase 2 of #9.

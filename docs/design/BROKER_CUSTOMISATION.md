# Broker Customisation: Design Study

**Status:** decided (Phase 3 of #9). The design is public API in `sub0pub.hpp` (namespace `sub0`, Phase 2; see
`MIGRATION.md`). The decision record, with the public API's measured results per acceptance case, is **section 9**.
The prototype it was proven with (`tests/design/broker_config/`, namespace `sub0x`) stays as the measured record. **Baseline:** [../PERFORMANCE_BASELINE.md](../PERFORMANCE_BASELINE.md).
**Related:** #4 (capacity, done), #5 (scoped broker / lifetime-safe dispatch), [TAGGED_TYPES_PROPOSAL.md](../TAGGED_TYPES_PROPOSAL.md).

This document records the requirements, the options considered, the recommendation, and exactly what
the prototype does and does not yet prove. It is written so an independent review can continue from it.

---

## 1. Requirements

Maintainer requirements, as stated in the design discussion:

| # | Requirement |
|---|---|
| R1 | **Performance and pay-for-what-you-use are key.** The default path must not get slower or larger (bar: `PERFORMANCE_BASELINE.md`). |
| R2 | Support **a generic default broker override** (project-wide) **plus a per-Data override**. The per-Data override is "a crucial bonus". |
| R3 | The override syntax must be **short**, because it has to live **with the Data type itself**, so that *every* usage and call site picks it up. |
| R4 | Alternatively, the core type stays `Broker<Data>`, with the specialised implementation chosen from a **central site**. That choice needs a deeper design pass. **Get it right first time and fully fledged, rather than building in new limitations.** |
| R5 | Must serve **heterogeneous multi-core and IPC systems**, with a way to define **specialised endpoints**, and be **Zephyr/embedded friendly**, especially nRF54 (Cortex-M33 application/radio cores plus RISC-V VPR coprocessors). |
| R6 | The "Sub0" name means the design should let the **compiler collapse dispatch** (inline and devirtualise). The baseline measures the worst case, and collapse is a tracked follow-up. The customisation design must not block it. |
| R7 | `SUB0PUB_REENTRANT_*` style choices ("safety", "checked", "none") should be selectable levels, with debug-build checking by default and release opt-in (done in #7 as macros; this design generalises them per type). |

Constraints derived from the baseline findings (`PERFORMANCE_BASELINE.md`, "Embedded findings"):

- **F1** `SUB0PUB_THREAD_SAFE` does not compile on arm-none-eabi (no `std::mutex`). A lock must be a pluggable type (Zephyr `k_spinlock`, IRQ lock, FreeRTOS critical section).
- **F2** Thread-local storage is mandatory today (`__aeabi_read_tp`), only to support `cancel()` and nested publish. It must be optional.
- **F3** `Publish<T>` has a virtual destructor whose body does nothing: a vptr per publisher, plus a dependency on `operator delete`.
- **F4** A `filter()` that nobody overrides still costs a virtual call per subscriber per publish.
- **F5** One global `SUB0PUB_MAX_SUBSCRIPTIONS` sizes every type's table (RAM is paid per type per slot).
- **F6** Configuring per TU with macros (as the tests do for capacity) is a one-definition-rule (ODR) violation. Today nothing detects it.

## 2. The core problem

Every `Subscribe<T>`, `Publish<T>` and `publish()` for a type `T`, in every translation unit (TU) and every
module, must agree on how `T` is brokered: which table, what capacity, which lock, which dispatch. The
configuration therefore has to be **a property of the type**, resolved identically wherever the type is
complete. Anything decided per call site (for example `Subscribe<T, MyBroker>`) lets two sites silently
disagree, which fails R3.

There are two separate questions, which must not be conflated:

1. **Policy: how `T` is brokered.** Static and per type. It must be the same everywhere, so it is bound to the type.
2. **Instance/scope: which broker instance a given subscriber joins** (#5 sessions, endpoints). This is
   legitimately chosen per site and can be a runtime decision, *if* the policy says the type is scoped.

## 3. Options considered for binding the policy to `T`

| Option | Syntax at the Data site | Consistency across TUs | Works for `int`, `std::`, third-party | Verdict |
|---|---|---|---|---|
| A. Global macros (today) | none (global only) | build-system flags only; per-TU overrides are ODR violations (F6) | yes, but global only | keep only as the **builtin default source** |
| B. Traits specialisation `template<> struct configure<T>` | ~2 lines, **global namespace only**, separate from the type | must be visible in every TU: forgetting it is silent undefined behaviour ("ill-formed, no diagnostic required") | **yes** | keep for types you cannot modify, **plus a debug detector** |
| C. ADL declaration `config<...> sub0_config(T*);` | 1 line in `T`'s namespace, declaration only | part of the type's namespace API; forgetting it is as easy as forgetting B | classes and **enums** you own | keep, for enums and "own it but don't want to edit it" |
| D. Member alias `using sub0_config = config<...>;` | **1 line inside the type** | **part of the type definition, so identical wherever `T` is complete** | classes only | **primary mechanism** |
| E. Central registry header listing every type | not at the Data site | consistent if force-included, but the registry must include every message header (dependency inversion, coupling) | yes | reject as primary; B inside the project config header covers the central need |
| F. Broker chosen per use site `Subscribe<T, B>` | at every site | **sites can disagree** | yes | reject for *policy*; keep the *instance* choice (Domain) per site |
| G. `Broker<Data>` facade over `BrokerImpl<Data, resolved Config>` | n/a (how the library consumes A–D) | does **not** make a mismatch defined: `Subscribe<Data>`'s bases and members depend on the configuration, so resolving differently in two TUs is an ODR violation (review finding 2) | n/a | adopt for policy (answers R4 for policy); implementation replacement is still open (section 7) |

**Global default (R2):** a project config header named by `SUB0X_CONFIG_HEADER` and set **by the build
system** (CMake `target_compile_definitions(... INTERFACE ...)`, or Zephyr Kconfig generation). The library
includes it once its option vocabulary exists. This is the familiar embedded pattern (`FreeRTOSConfig.h`,
`lwipopts.h`, mbedTLS config). Because the build system applies it, every TU agrees by construction. It
defines a type such as `struct ProjectDefaults : sub0x::with<sub0x::Builtin, ...> {};`.

**Fundamental and third-party payloads:** either B, or, preferably, `Tagged<Payload, Tag>` from the
tagged-types proposal, where the **tag** carries a member `sub0_config`. `Tagged<float, CoreTemp>` then
takes its configuration from `CoreTemp`, which gives a distinct channel with its own policy and no
wrapper boilerplate.

## 4. Recommendation

Resolution order for `config_t<T>`, evaluated wherever `T` is used:

1. **Per-Data, exactly one of** (a static assertion rejects configuring a type in two places):
   - **member alias**: `struct Imu { ...; using sub0_config = sub0::config<sub0::Capacity<2>>; };`
   - **ADL declaration**: `sub0::config<...> sub0_config(gps::Fix*);` next to the type, in its namespace
   - **traits**: `SUB0PUB_CONFIGURE(int, sub0::Capacity<32>);` for types you cannot modify, or a `Tagged` tag
2. **Project default** from `SUB0PUB_CONFIG_HEADER` (build-system defined).
3. **Builtin**: today's `SUB0PUB_*` macros, so existing users see no change.

`sub0::config<Opts...>` means "the project default with these options applied", so per-type overrides
layer on the project's choices rather than on the library's.

The library keeps `Broker<Data>` as the only name call sites use. Internally it is
`BrokerImpl<Data, config_t<Data>>`, which answers R4 for *policy*. Selecting a *different broker
implementation* or transport is not solved by this; see section 7.

**Consistent configuration visibility is a mandatory build contract.** `Subscribe<Data>`'s bases and members
depend on the configuration. A `Data` type that resolves to different configurations in two TUs is
therefore an ODR violation, which is undefined behaviour. Carrying the configuration in the implementation
type does not make that defined. The member-alias and ADL mechanisms satisfy the contract by construction,
because they are part of the type's definition. Traits specialisations and the project header must be
visible in every TU. A **debug-build registry** (`Registry<Data>`, shared by all TUs because it doesn't
depend on the configuration) is a *best-effort diagnostic*:
- it records a fingerprint of the configuration's effective values atomically, with an atomic
  compare-and-exchange;
- it reports a different fingerprint when one is observed at runtime.

It cannot catch every violation, and it is not a safety guarantee.

### Policy axes (what a configuration controls)

| Axis | Options (prototype) | Default | Addresses |
|---|---|---|---|
| Capacity | `Capacity<N>` | `SUB0PUB_MAX_SUBSCRIPTIONS` | F5, #4 |
| Dispatch | `Snapshot`, `Direct`, `DirectChecked` | from `SUB0PUB_REENTRANT_*` | R7 |
| Context | `ThreadLocalContext`, `StaticContext` (no TLS), `NoContext` (no `cancel()`, no per-publish context cost) | ThreadLocal | F2 |
| Lock | `LockWith<L>` with any `lock()`/`unlock()` type; `NoLock` is an empty base | NoLock, or `std::mutex` when `SUB0PUB_THREAD_SAFE` | F1 |
| Filter | `NoFilter` removes the `filter()` virtual from `Subscribe<T>` (overriding it is then a compile error) | enabled | F4 |
| Storage | `Global` (one table per type), `Scoped` (tables in `Domain<T>` instances passed at construction) | Global | #5 |

Every option is scored on measured cost, footprint and guarantees in [AXIS_SCORES.md](AXIS_SCORES.md), one option
at a time.

Invalid combinations are compile errors: a lock without Snapshot dispatch, Snapshot without a publish context
(its snapshot is kept safe through the dispatch frame; found by the independent review), a lock without `ThreadLocalContext`
(a `StaticContext` frame stack shared by concurrent publishers let one thread cancel another's publication; found by
the axis review), `DirectChecked` without a context, `cancel()` without a context, `Domain<T>` for a non-scoped type, and a scoped subscriber built
without a domain. `Publish<T>` loses its virtual destructor (F3): it becomes an empty handle for global storage.

### Planned axes the design must leave room for (not in the prototype)

- **Endpoints / routes (R5):** `Routes<Endpoint...>`, a per-type list of transports (such as a Zephyr IPC
  service endpoint) that the broker forwards to like an implicit subscriber. Zero cost when absent.
  Heterogeneous peers need explicit type IDs plus a `makeLayout<T>()` handshake.
- **Deferred dispatch:** a queue policy (Zephyr `k_msgq`) for ISR-safe publish and receive-context control.
- **Static subscriber sets (R6, collapse):** a type whose subscribers are known at compile time can
  dispatch by direct calls, reaching the baseline "collapse target" (6 instructions for 1 receiver
  and 13 for 8, against 60–72 and 207–247 today).
- **Quiescence (#5):** unsubscribing while a snapshot dispatch is running needs a policy. The snapshot
  path currently has the use-after-free that #5 describes. Candidates are an in-flight counter per domain,
  aware of the current thread so that a subscriber unsubscribing itself doesn't deadlock. `Domain<T>` also
  has no shutdown protocol yet (review finding 4, section 7).
- **Cross-module (DLL) storage:** a storage policy whose table lives in one exporting module.

## 5. The prototype: what is proven

`tests/design/broker_config/` (built and run by ctest in CI):

| File | Proves |
|---|---|
| `sub0x_broker.hpp` | the whole mechanism: vocabulary, resolution chain, `Broker<Data, Config>`, `Subscribe`/`Publish`/`publish`, `Domain`, `Tagged` |
| `project_config.hpp` | global default via `SUB0X_CONFIG_HEADER`, applied by CMake to every TU |
| `app_types.hpp` | every binding mechanism: member alias, ADL (struct and enum), traits (`int`), tagged, project default |
| `test_binding.cpp` | resolution (static assertions), per-type capacity, `trySubscribe` reclaim, lean dispatch, `cancel()`, `filter()`, scoped domains with no cross-talk, tagged channels, publisher has no vtable (`sizeof == 1`) |
| `test_multi_tu_a.cpp`, `test_multi_tu_b.cpp` | a subscriber in one TU receives a publish from another; both TUs see the same table and configuration; no mismatch reported |
| `mismatch_a.cpp`, `mismatch_b.cpp` | a deliberately forgotten traits specialisation in one TU is **detected** at runtime (debug registry) |
| `compile_fail/*.cpp` | seven misuse cases rejected at compile time with the intended diagnostic |
| `test_binding.cpp` (review fixes) | `cancel()` and `DirectChecked` are scoped to the dispatching table: another domain's subscriber cannot cancel this domain's dispatch, and publishing into another domain from a receiver is not re-entry. The registry fingerprint is value-based |
| `test_endpoints.cpp` | the review's worked example (section 7): two isolated sessions and the same type, two transports (synchronous pipe and bounded asynchronous queue), bidirectional ingress/egress with split horizon, rejection reports while local delivery continues, teardown during delivery on the same thread and from another thread, domain close, and an application-defined broker (`Implementation<SingleSubscriberBroker>`) with a route |
| `bench_sub0x.cpp` | each configuration bound to its own type in one binary, measured under the baseline's control conditions |
| `bench_axes.cpp`, `test_axes.cpp`, `footprint/fp_axis_*.cpp` | every option alone (cost, footprint) and the guarantee each option claims: [AXIS_SCORES.md](AXIS_SCORES.md) |
| `footprint/fp_sub0x_*.cpp` | footprint per configuration (host, Cortex-M33) via `tests/footprint/measure_footprint.py` |

### Measured against the baseline

Full results: [../perf/prototype-sub0x-2026-09.md](../perf/prototype-sub0x-2026-09.md). Built with no project header,
so `Default` is the Builtin configuration, the same policy as `sub0pub.hpp` today. These figures include the
endpoint/teardown machinery added for the review (section 7). Earlier figures from before that work are in the git history.

| instr/op (GCC 13) | 0 subscribers | 1 subscriber | 8 subscribers | create + destroy |
|---|---:|---:|---:|---:|
| **Baseline** Snapshot (default) | 38 | 72 | 247 | 61 |
| **Baseline** Direct unchecked | 39 | 60 | 207 | 61 |
| **Baseline** ThreadSafe (`std::mutex`) | 123 | 157 | 333 | 218 |
| sub0x Default (Snapshot, ThreadLocal, filter) | 31 | 74 | 228 | 73 |
| sub0x Direct | 46 | 61 | 166 | 73 |
| sub0x Direct + NoFilter | 41 | 52 | 129 | 73 |
| sub0x Lean (Direct, NoContext, NoFilter) | **9** | **33** | **96** | 65 |
| sub0x Locked (Snapshot, spin lock, uncontended) | 90 | 123 | 347 | 105 |
| Floor: virtual `receive()` loop | | 11 | 89 | |

| Cortex-M33 `-Os`, 1 type (publisher, subscriber, publish site) | text | `sizeof(Publish)` | needs TLS | needs memcpy |
|---|---:|---:|---|---|
| **Baseline** Snapshot (default) | 566 | 8 | yes | yes |
| sub0x Default | 566 | 1 | yes | yes |
| sub0x Direct | 530 | 1 | yes | no |
| sub0x Direct + StaticContext | 518 | 1 | **no** | no |
| sub0x Lean | **394** | 1 | **no** | no |

- **Zero-cost requirement (R1): met only partly.** The default configuration costs less than the baseline
  with 0 and 8 subscribers, but more with 1 subscriber (+2) and for create+destroy (+12). It is the same
  size on Cortex-M33. The regressions are the price of the new correctness guarantees and are recorded as
  known issues in **section 8**. The baseline has neither the guarantees nor the costs.
- Pay-for-what-you-use works per option: the lean configuration is 4.2× cheaper to publish with no
  subscribers, 2.6× cheaper with 8, and 30% smaller on Cortex-M33.
- `StaticContext` costs the same as TLS on x86 (segment-relative access) but removes `__aeabi_read_tp` on
  Cortex-M. Only target measurements show that difference.
- **Portability finding:** MSVC did not detect the ADL hook with a zero-argument deleted poison pill plus
  `void_t` partial-specialisation detection. The prototype now uses a deleted *template* poison pill
  `template<class T> void sub0_config(T*) = delete;` with overload-based detection, which works on GCC,
  Clang and MSVC.

### Not yet proven (next steps, in order)

1. **Zephyr lock type:** compile a `k_spinlock` adapter against Zephyr headers (or a stub).
2. **Integration plan (done in Phase 2):** `sub0x` is now `sub0::` in `sub0pub.hpp`, with the static wiring of #9;
   the migration is in `MIGRATION.md`, the public tests in `tests/config/` and `tests/wiring/`. The plan was: move `sub0x` into `sub0pub.hpp` as `sub0::`. Macros stay as the builtin source,
   and `detail::Broker<Data>` becomes the facade. Update `MIGRATION.md`:
   - `Publish<T>` is no longer polymorphic;
   - a `filter()` override can fail to compile when the type is configured with `NoFilter`;
   - IPC classes (`ForwardSubscribe` and friends) must be ported onto the facade.
3. **Collapse (#9):** static subscriber sets as a configuration axis.
4. **Design questions for review** (answered in Phase 2; recorded in section 9):
   - Should `config<>` layer on the project default or on Builtin? **The project default.** `config<Opts...>` is
     an alias of `with<GlobalDefault, Opts...>`, so it also names a different type wherever the default differs
     (PR #13 review finding 1).
   - Should a mismatch in release builds be link-time detectable? **Not solved.** It stays a build contract with
     a debug-build diagnostic (`SUB0PUB_CHECK_CONFIG`); known issue K6.
   - Naming: **kept** `sub0_config` for the member and ADL hook, `SUB0PUB_CONFIGURE` / `configure<T>` for
     traits, and the option names of section 4.
   - Should `Storage::Scoped` with a default domain also be allowed? **No.** A scoped type is always bound to
     an explicit `Domain<T>` (a scoped subscriber built without one does not compile). A type that needs a
     process-wide instance uses Global storage.
   - IPC: routes in the configuration or attached at runtime? **Both, at different places.** The runtime
     registry attaches endpoints at runtime (`Route<Data, Transport>`); the static wiring binds them at the
     composition point (`Forward`, `StaticForward`), which is the zero-cost form (section 9).
   - Should `Subscribe<T>` keep a virtual destructor? **No**, nor `Publish<T>`: both are protected and
     non-virtual (K7, K25).

## 6. How to continue (for the review session)

```bash
cmake --preset default && cmake --build --preset default && ctest --preset default   # 9 tests incl. prototype; also ci-tsan preset
python3 tests/bench/run_baseline.py build/tests          # baseline instr/op + ns/op
python3 tests/footprint/measure_footprint.py             # host + Cortex-M33 footprint
```

Open work items are listed in section 5 ("Not yet proven") and section 7 (review). The
collapse/devirtualisation follow-up (R6) is tracked in issue #9.

## 7. Review (PR #8, first API review of `1b1da32`)

**Verdict: not ready to freeze the v2 API.** The policy resolution and footprint work are worth keeping,
but the prototype customises the existing broker. It cannot yet provide a different broker implementation
or a transport endpoint (R5).

| # | Finding | Status |
|---|---|---|
| 1 | No implementation-selection hook, endpoint dependency or transport contract; `Routes<>` is future work | **Prototyped:** `Implementation<B>` hook plus a documented broker concept and `sub0x::kit`; `Route<Data, Transport>` bindings with the transport concept `SendResult send(const Data&)` |

| 2 | Cross-TU guarantee overstated; registry hashed the type name, not values; unsynchronised registry | **Fixed:** doc states a build contract; registry is value-based, atomic and best-effort (sections 3 and 4) |
| 3 | Scoped domains isolated subscriptions but not `cancel()` / `DirectChecked` | **Fixed:** the context is keyed by the table being dispatched; regression tests reproduce the reported case |
| 4 | Teardown: snapshot dispatch retains raw pointers; `Domain` has no shutdown protocol | **Prototyped:** activation, disconnect, quiescence, self-disconnect and `Domain::close()` contracts (below), ASan/TSan-tested |
| 5 | Transport results and routing: `void publish()` cannot report rejection; async payload ownership; echo loops | **Prototyped:** `PublishReport`, copy-at-acceptance rule, split horizon on ingress origin |

All five findings are now addressed in the prototype. The API is still not frozen. The prototype
answers are proposals for the maintainer, and their costs are listed in section 8.

**Separation of concerns agreed in review:**

| Concern | Where it belongs |
|---|---|
| Message/channel identity and default policy | Type-associated declaration or central traits (sections 3 and 4) |
| Broker implementation selection | Documented compile-time customisation hook |
| Concrete socket, IPC channel, queue or device | Endpoint instance supplied during construction |
| Session isolation and endpoint bindings | Explicit domain/session object |
| Disconnect and callback lifetime | Registration/connection contract |
| Wire identity and encoding | Explicit protocol/schema contract |

**Acceptance criterion before the API freeze** (now implemented in `test_endpoints.cpp`). A worked example covering:
- two isolated sessions and the same message type;
- two transport implementations;
- ingress and egress;
- transport rejection;
- teardown during delivery.

The example determines the public surface; no further policy options are added before it.

### Proposed answers to the review's questions (for maintainer decision)

1. **Replace the broker, add forwarding, or both?** Both, as two separate hooks:
   - an *implementation hook*: a configuration member such as `template<class D> using broker = ...`, satisfying a
     documented broker concept (`trySubscribe`, `unsubscribe`, `publish`, `cancel`), with the library broker as the default;
   - *forwarding*, which needs no broker replacement. Endpoints bind to a domain: egress as a subscriber-like
     sink, ingress by publishing into the domain.
2. **Endpoint ownership and multiple connections.**
   - The application owns endpoint instances.
   - A binding object (RAII) connects one endpoint to one domain for a set of types.
   - Several connections for the same type means several bindings, either in separate domains (isolated
     sessions) or in one domain (fan-out).
   - Static route declarations name *roles*; runtime bindings supply the instances.
3. **Publication guarantees.**
   - Local publish stays `void noexcept`, the cheap path. Endpoints expose a result: accepted, rejected
     (full or disconnected), or closed.
   - An opt-in reporting publish returns per-route acceptance.
   - **Acceptance is not remote delivery.**
   - Local delivery continues when a route rejects.
   - An asynchronous endpoint must copy or serialize the payload at acceptance and never keep a reference.
   - Ingress deliveries carry the origin binding in the dispatch context. Egress skips the origin
     ("split horizon"), which prevents echo loops.
4. **Teardown (finding 4).**
   - Unbinding or unsubscribing marks the entry inactive under the table lock, then waits for in-flight
     dispatches of that table (a per-table counter) before returning.
   - A subscriber unsubscribing itself from its own callback is detected through the dispatch context, and
     completes when that dispatch unwinds instead of waiting on itself.
   - `Domain` destruction first closes the domain to new bindings, then quiesces.

### Lifetime contracts as prototyped (finding 4)

- **Activation.** Single-threaded configurations register in the `Subscribe` constructor. Concurrent
  configurations (a `Lock`) do not, because another thread could otherwise dispatch into a half-constructed
  object. The most-derived constructor calls `trySubscribe()`; `Route` does so itself.
- **Disconnect.** After `disconnect()` returns, `receive()` is never called again, on any thread.
  - The base destructor disconnects too, but by then the derived object is gone. With concurrent publishers,
    call `disconnect()` first in the most-derived destructor; `Route` does so.
  - Same-thread: disconnecting (and destroying) a subscriber during a dispatch, including from its own
    `receive()`, is safe with Snapshot dispatch. The dispatch forgets it.
  - Concurrent: each dispatch registers its snapshot in the table's active list and publishes which
    subscriber it is calling. `disconnect()` nulls the subscriber in every active snapshot and waits only
    while another thread is inside *that* subscriber's callback. The wait is bounded by one callback and
    cannot starve.
  - A seq_cst store-then-load handshake (dispatcher: publish `current`, re-load the entry; writer: null the
    entry, load `current`) guarantees at least one side sees the other.
- **Domain.** `close()` rejects new subscriptions (`SubscribeResult::Closed`), drops publishes, detaches the
  current subscribers and waits out callbacks in progress. A `Domain` must outlive every handle bound to
  it; this is debug-checked by a handle count.

## 8. Known issues: compromises against correctness without cost

The design intent of the Sub0 libraries is **correctness without cost**. Wherever the prototype pays for
correctness, or accepts a limitation to avoid a cost, the compromise is recorded here with its measured
price (GCC 13 instructions per operation, Cortex-M33 bytes; see section 5), so it can be engineered away
rather than forgotten. Baseline issues in today's library are listed in
[PERFORMANCE_BASELINE.md](../PERFORMANCE_BASELINE.md) ("Embedded findings").

| # | Compromise | Measured cost | Why | Route to zero cost |
|---|---|---|---|---|
| K1 | Same-thread teardown safety on every create+destroy: an atomic registration flag, plus a thread-local check for in-progress dispatches that must forget the subscriber | create+destroy 73 vs 61 (+12); **since the opt-in default:** the atomic flag is a plain `bool` without a lock and the frame walk needs a publish context, so the default's create + destroy is 46 (v1.0: 48) | a subscriber destroyed during a dispatch must not be called by it | skip the check when no dispatch of that table is active on this thread (already a single thread-local load); make the flag non-atomic in single-threaded configurations |
| K2 | The dispatch frame carries `origin`, `report` and `snapshot` for routes and teardown even when a type has no routes | Snapshot, 1 subscriber: 74 vs 72 (+2); Direct, 0 subscribers: 46 vs 39 (+7); **since the opt-in default:** only types with a publish context (`SUB0PUB_CANCEL`, Snapshot, a lock) carry a frame; the default has none | split horizon, rejection reports and same-thread teardown need them | a `Routable` option: types without routes use a minimal frame |
| K3 | Concurrent configurations pay a seq_cst handshake per subscriber per publish, and a second lock acquisition to unlink the dispatch | Locked, 8 subscribers: 347 (spin lock) vs baseline ThreadSafe 333 (`std::mutex`; the lock types differ); public header with `SUB0PUB_THREAD_SAFE` (`std::mutex`): 1 / 8 subscribers 260 / 554 against 149 / 332 before Phase 2 (`docs/perf/compare-v1-v2-2026-09.md`) | disconnect-while-delivering safety, which the baseline ThreadSafe mode lacks: it has the #5 use-after-free | per-subscriber reference counts or epoch-based reclamation; measure against the handshake. **Measured** ([spikes/quiescence.md](spikes/quiescence.md)): hazard pointer and epoch are cheaper (8 subscribers 165 and 118 vs 256) only because they skip self-disconnect, nested-publish and thread-limit safety; they fail lifetime probes the handshake passes. Fixed (round 2) and given the same empty-table fast path, hazard costs 12/105/182 (0/1/8 subscribers) vs the handshake's 25/99/260, and epoch 25/84/130 (after the slot-ownership race fix, quiescence.md section 12); both bound threads and nesting and drop publications past the bound in release builds. The handshake stays; the fast path (a relaxed count mirror, -42 on an empty publish, +13 on create/destroy) is a candidate for this prototype |
| K4 | Concurrent `disconnect()` can block (bounded by one callback on another thread); two receivers disconnecting each other at the same moment from different threads deadlock | blocking; a documented usage rule | the only way to guarantee "no call after disconnect returns" without allocation | a non-blocking `disconnectLater()` for use inside receivers (prototyped: quiescence mechanism 4); a deadlock detector in debug builds |
| K5 | Concurrent configurations need an explicit `trySubscribe()` at the end of the most-derived constructor | API burden, easy to forget | the base constructor runs before the derived object exists | a CRTP `Subscriber<Derived, Data>` helper that activates after construction (prototyped: quiescence mechanism 5, heap objects only); a debug warning for a never-activated subscriber |
| K6 | `Domain` lifetime (it must outlive its handles) is only debug-checked; configuration consistency across TUs is a build contract with a best-effort diagnostic | undefined behaviour if violated in release | no zero-cost runtime mechanism exists | link-time detection research (section 5, question 4). **Narrowed in Phase 2:** types local to one TU may use different `SUB0PUB_*` macros in different TUs, because the Builtin default and `config<Opts...>` name a different type for each set of values (tested in both link orders, `Sub0Pub_ConfigIdentity_*`); the contract remains for a type shared between TUs |
| K7 | ~~`Subscribe<T>` keeps a virtual destructor~~ **Resolved in Phase 2:** `Subscribe<T>`'s destructor is protected and non-virtual and `Publish<T>` is not polymorphic, so `operator delete` is no longer a link dependency (Cortex-M33 one-type image 566 → 526 B text). Remaining cost: subscribers destroyed through their own type should be `final` to silence compiler warnings | – | – | – |
| K8 | Custom brokers (`Implementation<>`) support Global storage only; `Domain` requires the library broker | limitation | the prototype's scoped table type is library-internal | put the table type in the broker concept |
| K9 | Cancel, re-entrancy checks and teardown walk this thread's dispatch-frame stack | O(nesting depth), usually 1 | frames are per Data type, shared by all of that type's domains | per-table frame chains if deep nesting appears in practice |
| K10 | The cross-thread teardown test is probabilistic | a mutation (no wait in disconnect) is caught in about 4 of 5 runs | data races are timing-dependent | a deterministic interleaving harness (test hooks at the handshake points). The quiescence spike's widened race window caught its mutation 5 of 5 runs; `Sub0Pub_QxProbes` checks self-disconnect, nesting and thread-limit cases deterministically |
| K11 | ~~A pattern B publisher that stores its output costs gcc +9 publish instr, +40 B RAM~~ **Withdrawn (fairness review):** measured against static-address code, so it was the price of runtime binding, which hand-written code pays too. Holding the wiring by value, the stored-output publisher is `=` against hand-written runtime binding on every build ([COLLAPSE_EVIDENCE.md](COLLAPSE_EVIDENCE.md), "Fairness review") | none | - | - |
| K12 | ~~The `DynamicPort<T, N>` bridge costs gcc +1 publish instr; empty +3 / +11 / +6~~ **Withdrawn (fairness review):** against a hand-written registry with the same features (capacity, removal) it is `=` populated and empty; the empty-port cost is the price of accepting dynamic subscribers at all | none | - | - |
| K13 | C++20/23 spellings (concepts, deducing this, `std::expected` results) are optional extras; the C++17 spelling is the contract | none at runtime; diagnostics are poorer under C++17 SFINAE | GCC 13 and `arm-none-eabi-g++` 13 lack deducing this; clang + libstdc++ 13 lacks `std::expected` ([spikes/cxx23_upgrade.md](spikes/cxx23_upgrade.md)) | raise the baseline once the embedded toolchains (Zephyr SDK, nRF Connect SDK) ship GCC 14 |
| K14 | Capability routing is silent on a signature mismatch: a receiver whose `receive` does not accept the message (wrong parameter type, a non-const `receive` reached through a const binding) is not delivered to, with no diagnostic | a missed delivery, found only by tests | routing by capability is what lets receivers need no base class or registration | an opt-in `receives<T...>` declaration per receiver checked at bind time; a debug diagnostic when a bound receiver handles no message type ever published on its wiring (the opt-in guard `sub0::handles_v<R, T>` exists since the scores review, [COLLAPSE_SCORES.md](COLLAPSE_SCORES.md)) |
| K15 | Split horizon decides at compile time when the origin's type is bound once in the wiring; the origin must then be that bound endpoint | a debug assertion; undefined skip target if violated in release | otherwise an address compare per publish (+3 to +4 instr) | `publishFrom<Endpoint>(msg)`, which identifies the origin by type and needs no origin object |
| K16 | A Lock (concurrent publishers) requires `ThreadLocalContext`, so every concurrent configuration needs TLS (`__aeabi_read_tp` on Cortex-M; Zephyr `CONFIG_THREAD_LOCAL_STORAGE`) | a link dependency and per-thread TLS block on targets that otherwise avoid TLS | a process-wide `StaticContext` frame stack is unsafe with concurrent publishers (reproduced: lost deliveries and data races) | a context keyed by the RTOS thread (for example indexed by `k_current_get()`) as a third context option |
| K17 | Nothing is shared between `Data` types: each type instantiates its own table, dispatch loop and handle code | Cortex-M33: +558 B text and +53 B RAM per type (Default), +382 B / +49 B (Lean) ([AXIS_SCORES.md](AXIS_SCORES.md)) | the configuration is resolved per type at compile time | a type-erased dispatch core shared by all types with the same configuration (the loop over `Subscribe` bases does not depend on `Data`), leaving only the typed entry points per type |
| K18 | Pattern B1: split horizon between two endpoints of the same transport type is decided by an address compare at run time (the type identifies neither) | `transport_two_links`: publish +8 (gcc) / +10 (clang), Cortex-M33 path +10 against hand-written runtime code ([COLLAPSE_SCORES.md](COLLAPSE_SCORES.md)) | a runtime-bound endpoint's identity is a runtime value | give each link its own adapter type (`struct LinkA : sub0::Forward<Radio> { using Forward::Forward; };`) and use `publishFrom<LinkA>(msg)`: `=` on gcc and Cortex-M33 (clang: K23) |
| K19 | Pattern B cancellation is opt-in per call: a receiver returning `false` stops only `publishCancelable`; plain `publish()` ignores it silently, and `Sink<T>` (B3) has no cancelable publish | a missed stop, found only by tests (unit test "cancellation") | the cancel protocol is a return value, not shared state, so it is free but must be requested | a compile-time diagnostic when `publish()` reaches a bool-returning receiver, or a `Sink` constructed from `publishCancelable` |
| K20 | Pattern B2 binds complete objects with static storage duration only: no automatic storage, no array elements, no receiver added at run time, and a bound array binds nothing; a fan-out of N receivers of one type is N named objects, delivered unrolled | `cf_static_wiring_local`, `cf_static_wiring_element`; 32 receivers: +668 B Cortex-M33 text against a hand-written loop over an array (`many_receivers`) | C++17 template arguments must be addresses of complete objects; a fold expression has no loop form | B1 (`wire(...)`) for dynamic lifetimes; a `StaticArray<&array>` binding that delivers by loop (not prototyped) |
| K21 | `DynamicPort<T, N>` has no snapshot and no report: a receiver removed during delivery makes the next one miss that publication, `add()` at capacity drops silently (`tryAdd()` reports), and gcc/clang keep an out-of-line copy of `receive()` beside the inlined one | x86 image +48 B (gcc) / +30 B (clang), +68 B with churn; Cortex-M33 `=`; unit tests "limitation K21", "DynamicPort: capacity" | the port is the least a dynamic side can cost (the hand-written registry has the same removal hazard) | iterate a count snapshot or backwards; `BrokerPort` with Snapshot where removal during delivery is needed |
| K22 | A nested publication on the same static wiring (a receiver publishing on the wiring that binds it) is compiled as recursion through the fold | gcc x86: +536 B text and an out-of-line `sub0` function (publish -1.5 instructions); clang B2 publish +4; Cortex-M33 `=` (`nested_publish`) | the optimiser's inlining of a recursive template instantiation | inferred, not proven: mark the nested publication's entry `noinline`, or publish nested messages through a separate wiring |
| K23 | Clang does not propagate `Wiring` bindings held inside an aggregate the way it propagates a hand-written struct of pointers | one publisher holding two wirings: publish +3, RAM +24 B; per-link adapter types: publish +5 (clang only; gcc and Cortex-M33 `=`) | observed optimiser behaviour; storing pointers instead of references in `Wiring` did not change it | open: compare with clang 19+, or a `Wiring` that stores a plain struct instead of `std::tuple` |
| K24 | Pattern B cancellation combined with `filter()` is not byte-identical to hand-written code | Cortex-M33: +4 path instructions, +12 B text; gcc x86: +3 path instructions, publish +0.3 (`cancellation_filtered`, bool and token alike, observable form); clang `=` | the filter and the stop are two separate branches where hand-written code merges them | open (small); inferred to be a GCC block-ordering choice |
| K25 | `Publish<T>`'s destructor is protected and non-virtual, so a publisher is always a class derived from it: `sub0::Publish<T>` cannot be a plain variable (`struct Out final : sub0::Publish<T> {};`) | one line per publisher type; a compile error otherwise | deleting through `Publish<T>*` must not compile (v1 destroyed it virtually), and a virtual destructor costs a vptr per publisher and an `operator delete` link dependency (K7) | a library-provided final handle type, if direct handles prove common |
| K26 | Under Direct dispatch (the default), a table change during that table's own dispatch (subscribe, unsubscribe, `Domain::close()` from `receive()` or `filter()`) is detected only by the debug-build check (`DirectChecked`); a release build does not detect it | in a release build the rest of that publication may skip a subscriber or call one added during it | detection needs a publish frame, which the default does not carry (K2) | opt in with `sub0::Snapshot` / `SUB0PUB_REENTRANT_SAFE` for types that change their table from their own callbacks; `Domain::close()` clears slots so a dispatch in progress stops calling them either way |

## 9. Decision record (issue #9, Phase 3)

The v2 broker design is decided as below. Every result in this section is measured on the **public API**
(`include/sub0pub/sub0pub.hpp`, namespace `sub0`), not the prototypes: each chosen-model variant of the collapse
cases has a `sub0_*` form built on the public header
([../perf/collapse/phase3-public-api-2026-09.md](../perf/collapse/phase3-public-api-2026-09.md), GCC 13 and
Clang 18 at `-O2`, arm-none-eabi GCC 13 `-Os` for Cortex-M33, LTO pairs for the cross-file case).

**The public API reproduces the prototypes.** 324 public-variant measurements (case × build × form) have a
`sub0x` twin:
- **Static wiring, 270 rows** (B1, B2, B3, cancellation, split horizon, one publisher over two domains):
  identical to the prototype on every metric.
- **`DynamicPort`, 18 rows:** 14 identical; 4 are 1 B smaller (clang text).
- **Runtime registry, 36 rows** (`Domain`, `Route`, `BrokerPort`): 2 identical; 28 equal or cheaper on every
  metric, because the public default is the lean configuration and the protected destructors remove the
  `operator delete` dependency; 6 mixed: clang `BrokerPort` images +410 B text under churn (publish -4.5
  instructions) and +8 B when empty; and on Cortex-M33 the `Domain` publish path is 16 static instructions
  longer with one indirect call fewer (direct iteration inlined), in a smaller image with less RAM and no TLS or
  `operator delete`.

**The recommended publisher mixin is measured on the public API.** `sub0::Publisher<Derived, Out>` (`sub0_b1_mixin`
in eight cases, `sub0_alt2_crtp_mixin` in `publisher_ergonomics`) is identical to a publisher that stores its
output by hand (`sub0_b1_wire`, 48 of 48 rows) and to the prototype mixin (6 of 6).

### Decisions

| # | Decision | Chosen | Rejected, with the evidence |
|---|---|---|---|
| D1 | Where a type's broker policy lives | On the type: member alias `using sub0_config = sub0::config<...>`, ADL declaration or `SUB0PUB_CONFIGURE`; else the project header (`SUB0PUB_CONFIG_HEADER`); else the `SUB0PUB_*` macros (sections 3, 4) | a broker chosen per use site (sites can disagree); a central registry header (dependency inversion) |
| D2 | What the default costs | The cheapest correct dispatch: direct iteration (checked in debug builds), no publish context, no `filter()`, no lock. Each costlier feature is opt-in, and using it without opting in is a compile error or a debug-build report (`MIGRATION.md`, "The default is the cheapest dispatch") | the earlier v2 default with snapshot, `cancel()` and `filter()` always on: 77 / 287 instr/op against 38 / 101 for 1 / 8 subscribers (`docs/PERFORMANCE_BASELINE.md`) |
| D3 | The hot-loop structure (#9's central question) | Static wiring at the application's composition point: plain receiver classes, bound with `StaticWiring<&a, &b>` (static storage), `wire(a, b)` (runtime lifetimes) or behind a `Sink<T>` port (an ABI boundary) | policy switches inside the virtual registry, which keep its dispatch model (#9 review finding 3); routing static receivers through the registry: +78 to +86 publish instructions, +35 to +68 with no dynamic subscriber ([spikes/static_dynamic_bridge.md](spikes/static_dynamic_bridge.md)) |
| D4 | The static-to-dynamic boundary | Genuinely dynamic subscribers join the static wiring through `DynamicPort<T, N>`, or through `BrokerPort<T>` to the runtime registry when they need its policy (Snapshot, lock, domain) | an inverted bridge (registry in front of the static wiring): publish +37 / +10 populated, +7 / +15 empty, over the hand-written equivalent ([spikes/static_dynamic_bridge.md](spikes/static_dynamic_bridge.md)) |
| D5 | Cancellation in the static path | A receiver returns `bool` (`false` stops) and the publisher calls `publishCancelable`: no shared or thread-local state | a thread-local `cancel()` mirroring the registry: gcc +3 instructions; Cortex-M33 +14 path instructions, +257 B RAM and a TLS dependency ([spikes/static_cancellation.md](spikes/static_cancellation.md)) |
| D6 | Publisher ergonomics | Name the `StaticWiring` alias, or the CRTP mixin `Publisher<Derived, Out>`, or `Sink<T>` across a boundary | C++23 deducing this (GCC 13 rejects it; where it builds it costs exactly the mixin) ([spikes/publisher_ergonomics.md](spikes/publisher_ergonomics.md)) |
| D7 | Lifetime | `Subscribe<T>` and `Publish<T>` have protected non-virtual destructors (K7, K25); locked types register with an explicit `trySubscribe()` after construction (K5); `Domain::close()` detaches, rejects and quiesces; disconnect during a dispatch is safe under Snapshot | a virtual destructor (a vptr per object and `operator delete` on small targets); registration in the base constructor for concurrent types (another thread could dispatch into a half-built object) |
| D8 | Concurrency | `LockWith<L>`, any `lock()`/`unlock()` type, with the seq_cst disconnect handshake (K3) | hazard pointers and epochs: cheaper only because they skip self-disconnect, nesting and thread-limit safety ([spikes/quiescence.md](spikes/quiescence.md)) |
| D9 | Language standard | C++17 is the contract; C++20 concepts are an optional diagnostic aid (K13) | raising the baseline before the embedded toolchains ship GCC 14 ([spikes/cxx23_upgrade.md](spikes/cxx23_upgrade.md)) |

### Measured results per acceptance case (public API)

"=" means identical to the equal-work hand-written reference on every criterion, in both forms (observable and
removable work). Deltas are publish instructions per publication (gcc / clang) and Cortex-M33 image text,
against that reference. B1 is compared with hand-written runtime binding and B3 with a hand-written context
pointer plus function pointer (COLLAPSE_EVIDENCE.md, "The loop").

| Case | Static wiring (B2 `StaticWiring` / B1 `wire` / B3 `Sink`) | Runtime registry (public defaults) |
|---|---|---|
| Zero receivers | = / = / = | = |
| One receiver | = / = / = ¹ | +11 / +23; +416 B |
| Multiple receivers, repeated types | = / = / = ¹ | +56 / +46; +484 B |
| Default and runtime filters | = / = / = ¹: the always-true filter disappears, the runtime filter keeps its branch | with `SUB0PUB_FILTER`: +67.5 / +55.5; +544 B |
| Two independent domains | = / = / = ¹; one publisher over both domains: B2 =, B1 = except clang observable +3 (K23) | `Domain`: +71 / +63; +2232 B (the prototype's default: +147 / +193) |
| Concrete transport endpoint (egress + ingress) | = / = / = ¹, including the origin forms; two links of one transport type: B2 =, B1 +8 / +10 unless each link has its own type (K18) | `Route`: +116 / +113; +716 B and TLS (StaticContext: +688 B, no TLS) |
| Dynamic subscriptions | `DynamicPort` = when empty; populated = except an out-of-line `receive()` on x86 (K21) and, under churn, a longer gcc publish path | against a hand-written registry with the same features: +3.5 / -1.5; +32 B (v1.0: +19 / +37 at its leanest) |
| Cross-file, LTO off / on | = / = / = ¹ on every build, with and without LTO | +25 / +24 without LTO, +56 / +44 with LTO: LTO does not devirtualise the registry |

¹ B3 on clang fails only the static publish-path criterion: Clang inlines the type-erased call (no indirect call
remains, publish -1 to +0 instructions), and the metric then counts the inlined receiver as path instructions.

Remaining gaps, all recorded above: nested publication on one static wiring (K22), two links of one transport
type on clang (K18, K23), cancellation combined with `filter()` (K24), `DynamicPort`'s out-of-line `receive()`
(K21), and the `BrokerPort` bridge's setup, teardown and RAM (the price of its policy).

### Gate status (#9, "Done when")

| Criterion | Status |
|---|---|
| Every acceptance case passes the evidence standard on the named builds, with final-link evidence | **Met on GCC 13, Clang 18 and arm-none-eabi GCC 13 (Cortex-M33), with LTO pairs**, with the compromises listed. **Not met on MSVC x64:** behaviour is checked there (every variant builds and runs under ctest in CI), but the evidence tool reads ELF and callgrind output, and no `dumpbin`-based collector exists yet. RISC-V is deferred until a toolchain with a C++ standard library is in CI, as the issue allows |
| The recommended hot-loop pattern is documented and was chosen by measurement | Met: COLLAPSE_EVIDENCE.md ("Recommended hot-loop coding pattern"), `README.md` and `MIGRATION.md` (static wiring) |
| The dynamic-subscription path meets its regression limits | Met: the public registry costs +3.5 / -1.5 against a hand-written registry with the same features, against v1.0's +19 / +37, and v2's default publish is 34 / 104 instr/op for 1 / 8 subscribers against v1.0's 60 / 221 (`docs/perf/compare-v1-v2-2026-09.md`) |
| #8's broker API can select the static structure, and is frozen only after that | The public API provides both structures and the bridges between them. Freezing it is the maintainer's decision |
| Deterministic regression gating in CI (evidence standard) | Met: every public-API variant (`sub0_*`, `sub0pub_virtual*`, 510 build × form rows) has a recorded budget per metric in `tests/collapse/budgets.json`, and the CI `collapse-evidence` job fails on a breach or a missing budget (`--budgets`). Checked by injecting +100 instructions per publish and 4 KiB of RAM into `one_receiver/sub0_b2_static`: exit 1, both forms reported over budget. A deliberate change re-records the budgets (`--write-budgets`) in the same commit |

**Open before #9 can close:** MSVC final-link evidence.

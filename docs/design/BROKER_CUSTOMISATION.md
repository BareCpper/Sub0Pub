# Broker Customisation: Design Study

**Status:** design study, with a compiled and tested prototype (`tests/design/broker_config/`, namespace `sub0x`).
Not yet part of `sub0pub.hpp`. **Baseline:** [../PERFORMANCE_BASELINE.md](../PERFORMANCE_BASELINE.md).
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

Invalid combinations are compile errors: a lock without Snapshot dispatch, `DirectChecked` without a
context, `cancel()` without a context, `Domain<T>` for a non-scoped type, and a scoped subscriber built
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
| `compile_fail/*.cpp` | six misuse cases rejected at compile time with the intended diagnostic |
| `test_binding.cpp` (review fixes) | `cancel()` and `DirectChecked` are scoped to the dispatching table: another domain's subscriber cannot cancel this domain's dispatch, and publishing into another domain from a receiver is not re-entry. The registry fingerprint is value-based |
| `test_endpoints.cpp` | the review's worked example (section 7): two isolated sessions and the same type, two transports (synchronous pipe and bounded asynchronous queue), bidirectional ingress/egress with split horizon, rejection reports while local delivery continues, teardown during delivery on the same thread and from another thread, domain close, and an application-defined broker (`Implementation<SingleSubscriberBroker>`) with a route |
| `bench_sub0x.cpp` | each configuration bound to its own type in one binary, measured under the baseline's control conditions |
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
2. **Integration plan:** move `sub0x` into `sub0pub.hpp` as `sub0::`. Macros stay as the builtin source,
   and `detail::Broker<Data>` becomes the facade. Update `MIGRATION.md`:
   - `Publish<T>` is no longer polymorphic;
   - a `filter()` override can fail to compile when the type is configured with `NoFilter`;
   - IPC classes (`ForwardSubscribe` and friends) must be ported onto the facade.
3. **Collapse (#9):** static subscriber sets as a configuration axis.
4. **Design questions for review:**
   - Should `config<>` layer on the project default (current) or on Builtin? Current reasoning: per-type overrides should respect project choices.
   - Should a mismatch in release builds be link-time detectable? One idea is a per-configuration symbol and a weak/strong pairing trick. This needs research: no portable mechanism is known.
   - Naming: `sub0_config` for the member and ADL hook; `configure<T>` for traits; option names.
   - Should `Storage::Scoped` with a default domain also be allowed (a global instance plus opt-in scopes)?
   - IPC: should routes live in the per-type configuration (static) or be attached by endpoints at runtime (current `ForwardSubscribe` model)? Likely both: the static route list enables zero-cost dispatch tables.
   - Should `Subscribe<T>` keep a virtual destructor? See known issue K7 in section 8.

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
| K1 | Same-thread teardown safety on every create+destroy: an atomic registration flag, plus a thread-local check for in-progress dispatches that must forget the subscriber | create+destroy 73 vs 61 (+12) | a subscriber destroyed during a dispatch must not be called by it | skip the check when no dispatch of that table is active on this thread (already a single thread-local load); make the flag non-atomic in single-threaded configurations |
| K2 | The dispatch frame carries `origin`, `report` and `snapshot` for routes and teardown even when a type has no routes | Snapshot, 1 subscriber: 74 vs 72 (+2); Direct, 0 subscribers: 46 vs 39 (+7) | split horizon, rejection reports and same-thread teardown need them | a `Routable` option: types without routes use a minimal frame |
| K3 | Concurrent configurations pay a seq_cst handshake per subscriber per publish, and a second lock acquisition to unlink the dispatch | Locked, 8 subscribers: 347 (spin lock) vs baseline ThreadSafe 333 (`std::mutex`; the lock types differ) | disconnect-while-delivering safety, which the baseline ThreadSafe mode lacks: it has the #5 use-after-free | per-subscriber reference counts or epoch-based reclamation; measure against the handshake. **Measured** ([spikes/quiescence.md](spikes/quiescence.md)): hazard pointer and epoch are cheaper (8 subscribers 165 and 118 vs 256) only because they skip self-disconnect, nested-publish and thread-limit safety; they fail lifetime probes the handshake passes. Round 2 fixes and re-measures them |
| K4 | Concurrent `disconnect()` can block (bounded by one callback on another thread); two receivers disconnecting each other at the same moment from different threads deadlock | blocking; a documented usage rule | the only way to guarantee "no call after disconnect returns" without allocation | a non-blocking `disconnectLater()` for use inside receivers (prototyped: quiescence mechanism 4); a deadlock detector in debug builds |
| K5 | Concurrent configurations need an explicit `trySubscribe()` at the end of the most-derived constructor | API burden, easy to forget | the base constructor runs before the derived object exists | a CRTP `Subscriber<Derived, Data>` helper that activates after construction (prototyped: quiescence mechanism 5, heap objects only); a debug warning for a never-activated subscriber |
| K6 | `Domain` lifetime (it must outlive its handles) is only debug-checked; configuration consistency across TUs is a build contract with a best-effort diagnostic | undefined behaviour if violated in release | no zero-cost runtime mechanism exists | link-time detection research (section 5, question 4) |
| K7 | `Subscribe<T>` keeps a virtual destructor, so `operator delete` and `__cxa_pure_virtual` remain link dependencies | Cortex-M33: link dependency plus deleting-destructor code | subscribers are occasionally deleted through a base pointer | a protected non-virtual destructor (API decision). Measured ([spikes/embedded_size.md](spikes/embedded_size.md)): one-receiver image 5452/88/837 → 4632/8/813 B text/data/bss |
| K8 | Custom brokers (`Implementation<>`) support Global storage only; `Domain` requires the library broker | limitation | the prototype's scoped table type is library-internal | put the table type in the broker concept |
| K9 | Cancel, re-entrancy checks and teardown walk this thread's dispatch-frame stack | O(nesting depth), usually 1 | frames are per Data type, shared by all of that type's domains | per-table frame chains if deep nesting appears in practice |
| K10 | The cross-thread teardown test is probabilistic | a mutation (no wait in disconnect) is caught in about 4 of 5 runs | data races are timing-dependent | a deterministic interleaving harness (test hooks at the handshake points). The quiescence spike's widened race window caught its mutation 5 of 5 runs; `Sub0Pub_QxProbes` checks self-disconnect, nesting and thread-limit cases deterministically |
| K11 | A pattern B publisher that stores its output (`template<class Out>` with `Out&`, or the CRTP mixin) instead of naming the `StaticWiring` alias | gcc-O2 +9 publish instr, +40 B RAM; clang `=` ([spikes/publisher_ergonomics.md](spikes/publisher_ergonomics.md)) | the topology is not known where the publisher is written | name the alias where the topology is static; a call-site argument form avoids the stored reference (+24 B) |
| K12 | The `DynamicPort<T, N>` bridge from static wiring to runtime subscribers | gcc +1 publish instr; when empty gcc +3, clang +11, Cortex-M33 +6 path instr ([spikes/static_dynamic_bridge.md](spikes/static_dynamic_bridge.md)) | a runtime-sized set needs a count and a loop | omit the port from wirings with no dynamic side (the cost is only paid where it is bound) |
| K13 | C++20/23 spellings (concepts, deducing this, `std::expected` results) are optional extras; the C++17 spelling is the contract | none at runtime; diagnostics are poorer under C++17 SFINAE | GCC 13 and `arm-none-eabi-g++` 13 lack deducing this; clang + libstdc++ 13 lacks `std::expected` ([spikes/cxx23_upgrade.md](spikes/cxx23_upgrade.md)) | raise the baseline once the embedded toolchains (Zephyr SDK, nRF Connect SDK) ship GCC 14 |


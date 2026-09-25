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
| G. `Broker<Data>` facade over `BrokerImpl<Data, resolved Config>` | n/a (how the library consumes A–D) | config is part of the implementation's identity: a mismatch splits the table (defined behaviour) rather than corrupting memory | n/a | **adopt** (answers R4) |

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
`BrokerImpl<Data, config_t<Data>>`, which also answers R4. **The configuration is part of the
implementation type's identity**, so each configuration has distinct static state. A TU that resolves
differently therefore gets its own table: a functional split, not memory corruption. A **debug-build
registry** (`Registry<Data>`, which doesn't depend on the configuration and is therefore shared by all
TUs) records the first configuration fingerprint and reports any different one. That turns F6 and the
forgotten-traits hazard into a diagnosed error.

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
  aware of the current thread so that a subscriber unsubscribing itself doesn't deadlock.
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
| `bench_sub0x.cpp` | each configuration bound to its own type in one binary, measured under the baseline's control conditions |
| `footprint/fp_sub0x_*.cpp` | footprint per configuration (host, Cortex-M33) via `tests/footprint/measure_footprint.py` |

### Measured against the baseline

Full results: [../perf/prototype-sub0x-2026-09.md](../perf/prototype-sub0x-2026-09.md). Built with no project header,
so `Default` is the Builtin configuration, the same policy as `sub0pub.hpp` today.

| instr/op (GCC 13) | 0 subscribers | 1 subscriber | 8 subscribers | create + destroy |
|---|---:|---:|---:|---:|
| **Baseline** Snapshot (default) | 38 | 72 | 247 | 61 |
| **Baseline** Direct unchecked | 39 | 60 | 207 | 61 |
| sub0x Default (Snapshot, ThreadLocal, filter) | 37 | 68 | 194 | 59 |
| sub0x Direct | 38 | 52 | 150 | 59 |
| sub0x Direct + NoFilter | 35 | 46 | 123 | 59 |
| sub0x Lean (Direct, NoContext, NoFilter) | **9** | **33** | **96** | 59 |
| Floor: virtual `receive()` loop | | 11 | 89 | |

| Cortex-M33 `-Os`, 1 type (publisher, subscriber, publish site) | text | `sizeof(Publish)` | needs TLS | needs memcpy |
|---|---:|---:|---|---|
| **Baseline** Snapshot (default) | 566 | 8 | yes | yes |
| sub0x Default | 534 | 1 | yes | yes |
| sub0x Direct | 482 | 1 | yes | no |
| sub0x Direct + StaticContext | 458 | 1 | **no** | no |
| sub0x Lean | **382** | 1 | **no** | no |

- **Zero-cost requirement (R1): met.** The default configuration is at or below the baseline everywhere.
  Most of the difference is `Publish<T>` losing its vtable, plus codegen.
- Pay-for-what-you-use works: each option removes its own cost. The lean configuration is 4× cheaper to
  publish with no subscribers, about 2.5× cheaper with 8, and 32% smaller on Cortex-M33.
- `StaticContext` costs the same as TLS on x86 (segment-relative access) but removes `__aeabi_read_tp` on
  Cortex-M. Only target measurements show that difference.
- **Portability finding:** MSVC did not detect the ADL hook with a zero-argument deleted poison pill plus
  `void_t` partial-specialisation detection. The prototype now uses a deleted *template* poison pill
  `template<class T> void sub0_config(T*) = delete;` with overload-based detection, which works on GCC,
  Clang and MSVC.
- `operator delete` is still required through `Subscribe<T>`'s virtual destructor. A protected
  non-virtual destructor would remove it (subscribers are rarely deleted through a base pointer), but
  that is an API decision for review.

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
   - Should `Subscribe<T>` keep a virtual destructor? See the last bullet above.

## 6. How to continue (for the review session)

```bash
cmake --preset default && cmake --build --preset default && ctest --preset default   # 9 tests incl. prototype
python3 tests/bench/run_baseline.py build/tests          # baseline instr/op + ns/op
python3 tests/footprint/measure_footprint.py             # host + Cortex-M33 footprint
```

Open work items are listed in section 5 ("Not yet proven"). The collapse/devirtualisation follow-up (R6)
is tracked in issue #9.

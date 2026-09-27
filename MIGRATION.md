# v1 to v2 Migration Guide

This document tracks all breaking changes between Sub0Pub v1 and v2. Update this document with any commit that introduces a migration-relevant change.

---

## Build Requirements

| | v1 | v2 |
|---|---|---|
| C++ Standard | Claims C++11, actually needs C++17 | C++17 (declared correctly) |
| CMake Minimum | 3.7.1 | 3.21 |
| Build | `./configure && cmake --build ./build` | `cmake --preset default && cmake --build --preset default` |
| Test | N/A (no tests) | `ctest --preset default` |

**Action:** Update your `target_compile_features` to `cxx_std_17` if linking against Sub0Pub. CMakePresets are now the recommended way to configure/build/test.

---

## API Changes

### `Broker` moved to `sub0::detail`

The `Broker<Data>` class is now in `sub0::detail::Broker<Data>`. It was never intended as public API.

**Action:** If you referenced `sub0::Broker<Data>` directly, change to `sub0::detail::Broker<Data>`. Consider refactoring to avoid direct Broker access.

### `cancel()` free function signature changed

```cpp
// v1: required a dummy data argument for template deduction
template<typename From, typename Data>
void cancel(From& from, const Data& data);

// v2: Data type is now an explicit template parameter
template<typename Data, typename From>
void cancel(From& from);
```

**Action:** Change `sub0::cancel(obj, someData)` to `sub0::cancel<DataType>(obj)`.

### `receive()` and `filter()` are now `noexcept`

```cpp
// v1
virtual void receive(const Data& data) = 0;
virtual bool filter(const Data& data);

// v2
virtual void receive(const Data& data) noexcept = 0;
virtual bool filter(const Data& data) noexcept;
```

**Action:** Add `noexcept` to all `receive()` and `filter()` overrides. If your receive handlers can throw, wrap them in try/catch.

### `publish()` is now `noexcept`

The free functions `sub0::publish()` and `Publish<Data>::publish()` are now `noexcept`.

**Action:** Ensure no exceptions propagate out of subscriber `receive()` callbacks.

### `Subscribe<Data>::isSubscribed()` and `trySubscribe()` added

New `sub0::SubscribeResult` enum (`Subscribed`, `CapacityExceeded`). `Subscribe<Data>::isSubscribed()` reports whether the subscriber is in the per-type table. `Subscribe<Data>::trySubscribe()` retries registration and returns a `SubscribeResult`, leaving the table unchanged when it is full.

**Action:** None required. Check `isSubscribed()` where the number of subscribers per type cannot be bounded ahead of time.

### `Subscribe<Data>` and `Publish<Data>` have no virtual destructor

`Subscribe<Data>::~Subscribe()` is `protected` and non-virtual, and `Publish<Data>` has no destructor of its own: it is no longer polymorphic, and for global storage it is an empty handle. A subscriber is destroyed as its own type. Deleting one through a `Subscribe<Data>*` or `Publish<Data>*` is a compile error.

This removes the deleting-destructor code and the `operator delete` link dependency from small targets.

**Action:**
- Delete or hold subscribers by their own type.
- Mark leaf subscriber classes `final` when they are destroyed through a pointer to their own type (`delete`, `std::unique_ptr`) or held in `std::optional`. Otherwise gcc (`-Wdelete-non-virtual-dtor`) and clang (`-Wdelete-non-abstract-non-virtual-dtor`) warn.
- Remove `override` from subscriber destructors.

### `SUB0PUB_THREAD_SAFE` subscribers register explicitly

With a lock (`SUB0PUB_THREAD_SAFE`, or `sub0::LockWith<L>`), the `Subscribe<Data>` constructor no longer registers the subscriber. Registering there would let another thread call `receive()` before the derived class is constructed. After `disconnect()` returns, `receive()` is not called again on any thread.

**Action:** In the most-derived subscriber:
- call `trySubscribe()` at the end of the constructor;
- call `disconnect()` at the start of the destructor.

```cpp
struct Logger final : sub0::Subscribe<Sample>
{
    Logger() noexcept { trySubscribe(); }
    ~Logger() { disconnect(); }
    void receive(const Sample&) noexcept override;
};
```

Single-threaded configurations are unchanged: they register in the constructor.

### `SubscribeResult::Closed` added

`trySubscribe()` returns `Closed` when a scoped subscriber's `Domain` has been closed (see "Per-type configuration" below).

**Action:** A `switch` over `SubscribeResult` needs the new case.

### `Subscribe<Data>::cancel()` and `sub0::cancel<Data>()` are `noexcept`

`Subscribe<Data>::cancel()` is now `const noexcept`, and the free function `sub0::cancel<Data>(from)` is `noexcept`. `cancel()` outside a publication of that type does nothing; it no longer asserts.

**Action:** None.

### `detail::Broker<Data>` is an alias of the configured broker

`sub0::detail::Broker<Data>` now names the broker implementation that `Data`'s configuration selects; `cMaxSubscriptions` is still available. The broker's `unsubscribe()`, `isSubscribed()`, `active()`, `typeId()` and `typeName()` members are gone: use `Subscribe<Data>` and `Publish<Data>` instead.

**Action:** Remove direct `detail::Broker` use.

### Per-type configuration (new)

Each `Data` type can now choose its own policy. The `SUB0PUB_*` macros are the default for types that don't. The configuration is resolved once per type, from exactly one of:
- a member alias: `struct Imu { ...; using sub0_config = sub0::config<sub0::Capacity<2>, sub0::NoFilter>; };`
- an ADL declaration in the type's namespace: `sub0::config<sub0::Direct> sub0_config(Gps*);`
- `SUB0PUB_CONFIGURE(Type, options...)`, for types you cannot modify;
- otherwise the project default (`SUB0PUB_CONFIG_HEADER`), otherwise the `SUB0PUB_*` macros.

The options are:
- `Capacity<N>`;
- `Snapshot`, `Direct` or `DirectChecked`;
- `ThreadLocalContext`, `StaticContext` (no TLS) or `NoContext`;
- `LockWith<L>`, which also selects `Snapshot` and `ThreadLocalContext`;
- `Filter` or `NoFilter`;
- `Scoped`, with `Domain<Data>` sessions;
- `Implementation<Broker>`, for an application-defined broker.

`Route<Data, Transport>` binds a transport endpoint to a table. `sub0::publish(from, data, report)` reports what each route accepted. Invalid combinations are compile errors.

Every translation unit must resolve the same configuration for a type: resolving it differently is an ODR violation. A debug-build check (`SUB0PUB_CHECK_CONFIG`) reports mismatches it observes.

**Action:** None for the mechanism itself; see "The default is the cheapest dispatch" below for what the default now includes. Scored options: [docs/design/AXIS_SCORES.md](docs/design/AXIS_SCORES.md).

### Static wiring (new)

`sub0::wire(a, b, logger)` and `sub0::StaticWiring<&a, &b, &logger>` bind receivers at the application's composition point. Receivers are plain classes with a non-virtual `receive(const T&)`; each delivery is a direct call, measured equal to hand-written code. The rest of the static wiring API:
- `publishCancelable()` stops at a receiver whose `bool receive()` returns `false`;
- `Sink<T>` is a type-erased port for non-template publishers;
- `Publisher<Derived, Out>` is a CRTP mixin;
- `Forward<Transport>` and `StaticForward<&transport>` are transport endpoints, with split horizon through `publishFrom()`;
- `DynamicPort<T, N>` and `BrokerPort<T>` bring runtime subscribers into a static wiring;
- `handles_v<R, T>` asserts that a receiver handles a message.

**Action:** None. Scored forms: [docs/design/COLLAPSE_SCORES.md](docs/design/COLLAPSE_SCORES.md).

---

## Behavioral Changes

### Subscription capacity is a reported outcome, not an assertion

In v1 and earlier v2, exceeding `SUB0PUB_MAX_SUBSCRIPTIONS` only triggered `assert()`. With `NDEBUG` the broker wrote past its fixed table, and the next `publish()` overflowed its snapshot buffer. Now the extra subscriber is constructed but not registered: `isSubscribed()` returns `false`, it never receives data, and destroying it is a no-op. This is the same in debug and release builds. The capacity assertion has been removed, so debug builds no longer abort.

**Action:** Code that relied on the debug assertion to detect over-subscription should check `isSubscribed()` instead.

### Subscription order preserved on removal

In v1, `unsubscribe()` used swap-with-last, silently reordering the subscriber list. In v2, order is preserved using `std::move` of the tail.

**Action:** No code change needed. If you relied on the v1 reordering behavior (unlikely), be aware that order is now stable.

### Publish cancel flag redesigned

In v1, `publishCanceled_` was a per-`Broker`-instance `mutable bool`. In v2, it is a `thread_local bool` scoped to the active publish invocation, with save/restore for re-entrant calls.

**Action:** No code change needed.

### `Publish::publish()` is now `protected`

The member function `Publish<Data>::publish(data)` is no longer public. Use the free function `sub0::publish(this, data)` from derived classes.

**Action:** Replace `myPublisher.publish(data)` with `sub0::publish(myPublisher, data)`, or call `publish(data)` from within the derived class (protected access).

### The default is the cheapest dispatch; costly features are opt-in and detected

A type without its own configuration dispatches with the cheapest correct loop: direct iteration over the table, no
publish context, no `filter()`, no lock. Each feature that costs something is opt-in, per type or for every type.
Using one without opting in is caught at compile time or by a debug-build check:

| Feature | Opt in for every type | Per type | Without it |
|---|---|---|---|
| Subscribe or unsubscribe a type from its own `receive()` (including destroying the subscriber) | `SUB0PUB_REENTRANT_SAFE` | `sub0::Snapshot` | Reported in debug builds (`SUB0PUB_REENTRANT_CHECK`) |
| `cancel()`, `Route`, publish reports | `SUB0PUB_CANCEL` | `sub0::ThreadLocalContext` (or `StaticContext`) | Compile error naming the opt-in |
| `filter()` | `SUB0PUB_FILTER` | `sub0::Filter` | Compile error: a subscriber declaring `filter()` does not compile, even without `override` |
| Publishing or subscribing from several threads at once | `SUB0PUB_THREAD_SAFE` | `sub0::LockWith<L>` | Reported in debug builds (`SUB0PUB_THREAD_CHECK`) |

Earlier v2 builds defaulted to snapshot dispatch with `cancel()` and `filter()` always available.

A nested publish of the same type from `receive()` is supported under every dispatch. `SUB0PUB_REENTRANT_CHECK` no
longer reports it; it reports only a change to the table being dispatched.

**Action:** Build in debug and fix what is reported:
- add `SUB0PUB_FILTER` or `sub0::Filter` where `filter()` is overridden;
- add `SUB0PUB_CANCEL` or `sub0::ThreadLocalContext` where `cancel()` is called;
- add `SUB0PUB_REENTRANT_SAFE` or `sub0::Snapshot` where a subscriber of a type is created or destroyed from that
  type's `receive()`;
- add `SUB0PUB_THREAD_SAFE` or `sub0::LockWith<L>` where a type is used from several threads at once.

To restore the earlier v2 behaviour for every type, define `SUB0PUB_REENTRANT_SAFE`, `SUB0PUB_CANCEL` and
`SUB0PUB_FILTER` as `true`.

### Disconnecting during a dispatch is safe with Snapshot dispatch

A subscriber disconnected or destroyed while its type is being dispatched on the same thread is removed from that dispatch. Before, the dispatch's snapshot still held it and called it afterwards (issue #5). `filter()` may also disconnect or destroy its own subscriber: `receive()` is then not called.

Each publication's `cancel()` and the `DirectChecked` re-entrancy check apply only to the table being dispatched. That is one per type, or one per `Domain`.

**Action:** None.

### `SUB0PUB_TYPEIDNAME` compiles

`SUB0PUB_TYPEIDNAME` did not compile (issue #11). It does now: the identity passed to a `Subscribe`/`Publish` constructor names the type in stream headers. `Publish<Data>::typeName()` and `typeId()` are now public.

**Action:** None.

### Type ID fallback uses compile-time hash

In v1, all types without `SUB0PUB_TYPEIDNAME` received a hardcoded ID of `12345`, making IPC with multiple types silently broken. In v2, `utility::typeHash<Data>()` generates a unique compile-time hash per type using `__PRETTY_FUNCTION__` / `__FUNCSIG__`.

**Action:** No code change needed. IPC now works correctly without `SUB0PUB_TYPEIDNAME`. Note that type hashes are not stable across different compilers — use `SUB0PUB_TYPEIDNAME` for cross-compiler IPC.

---

## Removed Features

### `SUB0_EXPERIMENTAL` removed

The `SUB0_EXPERIMENTAL` flag and `publish_cstatic()` have been removed.

**Action:** If you used `sub0::publish_cstatic(data)`, create a static `Publish<Data>` instance yourself:
```cpp
static sub0::Publish<Data> publisher;
sub0::publish(publisher, data);
```

### `SUB0_BROKERSTATE` macro removed

The commented-out `SUB0_BROKERSTATE` macro has been removed. C++17 `inline static` handles this.

**Action:** Remove any `SUB0_BROKERSTATE` invocations.

---

## Configuration Changes

### `SUB0PUB_MAX_SUBSCRIPTIONS` (new in v1.0, carried to v2)

The fixed `cMaxSubscriptions = 8` is now configurable via `#define SUB0PUB_MAX_SUBSCRIPTIONS N` before including the header.

### `SUB0PUB_THREAD_SAFE` (new in v1.0, carried to v2)

Define `SUB0PUB_THREAD_SAFE true` to enable mutex-guarded subscribe/unsubscribe/publish operations.

### `SUB0PUB_REENTRANT_SAFE` (new in v2)

Default `false`. With `true`, `publish()` snapshot-copies the subscriber list before dispatching, so a subscriber may subscribe or unsubscribe (or destroy) a subscriber of the same type from within `receive()`. It also gives every type a publish context (`cancel()`).

### `SUB0PUB_CANCEL`, `SUB0PUB_FILTER` (new in v2)

Default `false`. `SUB0PUB_CANCEL` gives every type a publish context: `cancel()`, `Route` and publish reports. `SUB0PUB_FILTER` gives every subscriber a `filter()`. Per type: `sub0::ThreadLocalContext`, `sub0::Filter`.

### `SUB0PUB_THREAD_CHECK` and `SUB0PUB_THREAD_VIOLATION` (new in v2)

Without a lock, a `Data` type must not be published, subscribed or unsubscribed from two threads at once. `SUB0PUB_THREAD_CHECK` (default: on without `NDEBUG`) detects such an overlap and calls `SUB0PUB_THREAD_VIOLATION(what)`, which by default asserts, then aborts. It uses a `thread_local`; define it `false` on a target without thread-local storage.

### `SUB0PUB_REENTRANT_CHECK` and `SUB0PUB_REENTRANT_VIOLATION` (new in v2)

Without snapshot dispatch, a `receive()` that subscribes or unsubscribes (or destroys) a subscriber of its own `Data` type on the same thread is unsupported. `SUB0PUB_REENTRANT_CHECK` detects it and calls `SUB0PUB_REENTRANT_VIOLATION(what)`, which by default asserts and then aborts. A nested publish is supported and not reported. The check defaults to on in debug builds (`SUB0PUB_ASSERT` without `NDEBUG`) and off in release builds; it costs a `thread_local` frame per publish. Define `SUB0PUB_REENTRANT_CHECK true` to keep it in release. It has no effect with snapshot dispatch.

**Action:** A debug build that hits the abort relies on unsupported behaviour. Enable `SUB0PUB_REENTRANT_SAFE` (or `sub0::Snapshot` for the type) or restructure the subscriber.

### Configuration macros for per-type configuration (new in v2)

- `SUB0PUB_CONFIG_HEADER`: a header, named by the build system, that may define the project default configuration as `SUB0PUB_DEFAULT_CONFIG`.
- `SUB0PUB_DEFAULT_CONFIG`: the project default configuration type, e.g. `struct ProjectDefaults : sub0::with<sub0::Builtin, sub0::Capacity<4>> {};`.
- `SUB0PUB_CONFIGURE(Type, options...)`: configure a type you cannot modify.
- `SUB0PUB_CHECK_CONFIG`: the debug check for a type configured differently in two translation units (default: on without `NDEBUG`).
- `SUB0PUB_CONFIG_MISMATCH(what)`: the action when that check fails (default: assert, then abort).
- `SUB0PUB_DOMAIN_LIFETIME(what)`: the action when a `Domain` is destroyed while handles are still bound to it (default: assert, then abort).

The `SUB0PUB_*` policy macros must agree in every translation unit that uses a type. Setting them differently in one translation unit is only valid for types local to it.

### `SUB0_STRINGIFY` renamed to `SUB0PUB_STRINGIFY`

The macro was renamed for prefix consistency. The old name no longer exists.

**Action:** Replace `SUB0_STRINGIFY(x)` with `SUB0PUB_STRINGIFY(x)`.

---

## IPC Improvements

### BinaryReader now validates the prefix magic

In v1, the SUB0 magic prefix was read but never checked. In v2, the prefix is validated via `memcmp` against the expected value. A mismatch enters `SyncLost` state.

### Unknown typeIds are skipped instead of crashing

In v1, an unrecognized typeId in the stream caused a throw/assert with no recovery. In v2, the payload bytes (+ postfix) are discarded and the reader continues to the next frame. This allows version-skewed peers to coexist.

### SyncLost has a recovery path

In v1, `SyncLost` was a permanent dead end. In v2, the reader byte-scans forward for the next valid prefix magic and re-enters normal reading.

### `paddingSize` type widened

Changed from `int_least16_t` to `int32_t` to prevent overflow on large payloads.

---

## IPC Design Notes

### Endianness

Sub0Pub does **not** perform per-message byte-swapping. All peers on a given IPC channel must share the same byte order. This is by design -- runtime endianness conversion would contradict the library's zero-overhead principle. For mixed-architecture deployments, a connection-time layout verification handshake is planned for a future phase, with full type introspection via [Sub0Reflect](https://github.com/CraigHutchinson/Sub0Reflect).

### Type Layout Verification

The serialization protocol transmits raw bytes (`reinterpret_cast` of the data struct). Both peers must have identical struct layout. v2 provides `makeLayout<T>()` for connection-time verification:

```cpp
// Automatic -- no macros, no member lists
auto layout = sub0::utility::makeLayout<MyStruct>();
// layout.fingerprint: sizeof + alignof + arity + array info
// layout.layoutHash: per-member offset+size hash (GCC/Clang; 0 on MSVC)

// Compare layouts between peers at connection time
if (localLayout != remoteLayout) { /* reject connection */ }
```

`memberCount<T>` (and so the fingerprint arity) counts an array member as one member. Earlier v2 builds counted each array element separately (`struct { float d[4]; int t; }` reported 5, not 2). As a result, fingerprints of structs that contain arrays differ from those builds, and GCC now compiles `makeLayout<T>()` for such structs.

On GCC/Clang, the layout hash captures per-member offset and size via structured bindings (Boost.PFR-style), recursively fingerprinting nested structs and arrays. On MSVC, only the fingerprint (sizeof+alignof+arity) is available due to a compiler bug with structured bindings in template specializations. Full MSVC support is planned for C++26 reflection.

---

## Performance: v1.0 compared with v2

Measured with `python3 tests/compare/compare_versions.py`. It builds the same scenarios against the v1.0 tag's
header and the v2 header: each macro policy, selected per-type configurations and the static wiring, with
hand-written code as the floor. Full tables, both compilers and method notes are in
[docs/perf/compare-v1-v2-2026-09.md](docs/perf/compare-v1-v2-2026-09.md). The metric is exact instructions per
operation under callgrind, the repository's regression bar, not wall-clock time.

### Runtime (instr/op, gcc 13 -O2; clang 18 in brackets)

| Implementation | publish, 1 subscriber | publish, 8 subscribers | 8, first cancels | create + destroy |
|---|---:|---:|---:|---:|
| **v1.0** | 60 (60) | 221 (214) | 60 (57) | 48 (31) |
| v2 default (Snapshot) | 80 (73) | 290 (262) | 102 (91) | 80 (72) |
| v2 `SUB0PUB_REENTRANT_SAFE=false` (Direct) | 67 (61) | 242 (229) | 73 (64) | 80 (72) |
| v1.0 ThreadSafe | 130 (133) | 291 (287) | 130 (130) | 204 (192) |
| v2 ThreadSafe | 260 (266) | 554 (516) | 301 (281) | 310 (301) |
| v2 per-type config, Lean (`Direct, NoContext, NoFilter`) | 34 (29) | 104 (99) | n/a | 67 (61) |
| v2 static wiring (`StaticWiring`, `wire()`) | 8–9 (7–9) | 37 (37–40) | 15–16 (8–10) | n/a |
| hand-written direct calls | 9 (7) | 37 (37) | 15 (8) | n/a |

### Embedded footprint (Cortex-M33, one publisher, one subscriber, one publish site)

| Implementation | text / data / bss (bytes) | Needs thread-local storage | Other link-time dependencies |
|---|---|---|---|
| **v1.0** | 418 / 4 / 76 | yes | `operator delete` |
| v2 default (Snapshot) | 526 / 4 / 62 | yes | `memcpy`, `memmove`, `__cxa_pure_virtual` |
| v2 `SUB0PUB_REENTRANT_SAFE=false` (Direct) | 494 / 4 / 62 | yes | `memmove`, `__cxa_pure_virtual` |
| v2 per-type config, Lean | 350 / 4 / 49 | no | `memmove`, `__cxa_pure_virtual` |
| v2 `StaticWiring` | 12 / 0 / 4 | no | none |

### What this means when migrating

- **v2's default costs more than v1.0 at runtime.** Publishing to subscribers costs 20 to 69 instructions more on gcc
  (13 to 48 on clang); publishing to none is cheaper. Creating and destroying a subscriber costs 32 more on gcc (41 on
  clang). What that buys:
  - re-entrant publish, subscribe and unsubscribe (the snapshot);
  - capacity reported to the caller;
  - order-preserving unsubscription;
  - disconnect and destruction during a dispatch, and inside `filter()`, without use-after-free;
  - `cancel()` isolated per table.

  v1.0 had none of these.
- **The embedded image is 108 bytes larger than v1.0, and no longer needs `operator delete`.** That is 40 bytes
  smaller than v2 before the per-type broker, because `Subscribe`/`Publish` lost their virtual destructors.
- **`SUB0PUB_REENTRANT_SAFE=false` does not reach v1.0's cost any more.** It still saves 13 to 48 instructions per
  publish against v2's default on gcc.
- **For v1.0's cost or less, configure the type.**
  - A type configured `Direct, NoContext, NoFilter` publishes to 8 subscribers in under half v1.0's instructions,
    needs no thread-local storage, and is 68 bytes smaller than v1.0. It gives up `cancel()` and `filter()`.
  - Where the receivers are known when the application is composed, static wiring costs exactly what hand-written
    calls cost: about 6 times fewer instructions than v1.0 for 8 subscribers, and 12 bytes of code.
- **`SUB0PUB_THREAD_SAFE` now costs about twice v1.0's.** v1.0 is 130 and 291 instructions for 1 and 8 subscribers;
  v2 is 260 and 554.
  - That buys teardown that is safe during concurrent delivery. v1.0's and the earlier v2's ThreadSafe mode could
    call a subscriber after it was destroyed.
  - The cost is a handshake per subscriber per publish and a second lock acquisition per publish (known issue K3 in
    [docs/design/BROKER_CUSTOMISATION.md](docs/design/BROKER_CUSTOMISATION.md)).
  - A lighter lock than `std::mutex`, through `LockWith<L>`, closes most of the gap. A spin lock measures 134 and
    421 instructions, against v1.0's 130 and 291.
  - `SUB0PUB_THREAD_SAFE` still does not build on `arm-none-eabi`, which has no `std::mutex`. Use `LockWith<L>` with
    the RTOS lock.

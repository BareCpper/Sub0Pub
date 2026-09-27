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

### Re-entrant publish safety is now snapshot-based

`Broker::publish()` snapshot-copies the subscriber list before dispatching, preventing deadlock when a subscriber publishes the same type from within `receive()`. This adds ~1.5ns overhead per publish. Disable with `#define SUB0PUB_REENTRANT_SAFE false` if re-entrant publish is guaranteed not to occur.

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

Default `true`. Controls whether `publish()` snapshot-copies the subscriber list before dispatching. Set `false` to skip the snapshot for ~1.5ns faster publish if you guarantee no subscriber will re-entrantly publish the same type from within `receive()`.

### `SUB0PUB_REENTRANT_CHECK` and `SUB0PUB_REENTRANT_VIOLATION` (new in v2)

With `SUB0PUB_REENTRANT_SAFE false`, a `receive()` that publishes, subscribes or unsubscribes its own `Data` type on the same thread was silently unsupported. `SUB0PUB_REENTRANT_CHECK` now detects it and calls `SUB0PUB_REENTRANT_VIOLATION(what)`, which by default asserts and then aborts. The check defaults to on in debug builds (`SUB0PUB_ASSERT` without `NDEBUG`) and off in release builds. Define `SUB0PUB_REENTRANT_CHECK true` to keep it in release. The check has no effect when the snapshot is active (`SUB0PUB_REENTRANT_SAFE` or `SUB0PUB_THREAD_SAFE`).

**Action:** Only affects `SUB0PUB_REENTRANT_SAFE false` builds. A debug build that hits the new abort was already relying on unsupported behaviour. Either enable `SUB0PUB_REENTRANT_SAFE` or restructure the subscriber.

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

## Performance: v1.0 compared with v2 and the designs under review

Measured with `python3 tests/compare/compare_versions.py`, which builds the same scenarios against the v1.0 tag's
header, the v2 header's policies, the per-type broker prototype (#8) and the static wiring prototypes (#9), with
hand-written code as the floor. Full tables, both compilers and method notes:
[docs/perf/compare-v1-v2-2026-09.md](docs/perf/compare-v1-v2-2026-09.md). The metric is exact instructions per
operation under callgrind (the repository's regression bar), not wall-clock time.

Rows marked *prototype* are designs under review in `tests/design/` and `tests/collapse/`; they are not part of
`sub0pub.hpp` yet and their API may change.

### Runtime (instr/op, gcc 13 -O2; clang 18 in brackets)

| Implementation | publish, 1 subscriber | publish, 8 subscribers | 8, first cancels | create + destroy |
|---|---:|---:|---:|---:|
| **v1.0** | 60 (60) | 221 (214) | 60 (57) | 48 (31) |
| v2 Snapshot (default) | 75 (68) | 257 (228) | 90 (78) | 63 (47) |
| v2 Direct unchecked | 57 (61) | 211 (215) | 56 (57) | 63 (47) |
| v1.0 ThreadSafe | 130 (133) | 291 (287) | 130 (130) | 204 (192) |
| v2 ThreadSafe | 149 (144) | 332 (305) | 165 (155) | 220 (204) |
| *prototype* per-type broker, Default | 80 (73) | 290 (262) | 102 (91) | 80 (72) |
| *prototype* per-type broker, Lean | 34 (29) | 104 (99) | n/a | 67 (61) |
| *prototype* static wiring (`StaticWiring`, `wire()`) | 9 (7–9) | 37 (37–40) | 15–16 (8–10) | n/a |
| hand-written direct calls | 9 (7) | 37 (37) | 15 (8) | n/a |

### Embedded footprint (Cortex-M33, one publisher, one subscriber, one publish site)

| Implementation | text / data / bss (bytes) | Needs thread-local storage | Other link-time dependencies |
|---|---|---|---|
| **v1.0** | 418 / 4 / 76 | yes | `operator delete` |
| v2 Snapshot (default) | 566 / 4 / 77 | yes | `memcpy`, `memmove`, `operator delete`, `__cxa_pure_virtual` |
| v2 Direct unchecked | 526 / 4 / 77 | yes | `memmove`, `operator delete`, `__cxa_pure_virtual` |
| *prototype* per-type broker, Default | 570 / 4 / 62 | yes | as v2 Snapshot |
| *prototype* per-type broker, Lean | 394 / 4 / 58 | no | `memmove`, `operator delete`, `__cxa_pure_virtual` |
| *prototype* `StaticWiring` | 12 / 0 / 4 | no | none |

### What this means when migrating

- **v2's default costs more than v1.0.** Publishing to 1 or 8 subscribers costs 8 to 36 instructions more, and the
  embedded image is 148 bytes larger. That buys re-entrant-safe dispatch (the snapshot), bounded capacity reported to
  the caller, and order-preserving unsubscription. v1.0 had none of these.
- **To keep v1.0's cost** where you never publish, subscribe or unsubscribe a type from inside its own `receive()`,
  build with `SUB0PUB_REENTRANT_SAFE=false`. Publishing is then at or below v1.0 (gcc 57 vs 60, 211 vs 221). Creating
  and destroying a subscriber stays about 15 instructions dearer, and the image stays 108 bytes larger than v1.0's.
  `SUB0PUB_REENTRANT_CHECK` reports misuse in debug builds.
- **`SUB0PUB_THREAD_SAFE`** costs 19 to 41 instructions more than v1.0's on gcc (11 to 25 on clang), because v2 also takes the snapshot inside the
  lock. It still does not build on `arm-none-eabi` (no `std::mutex`).
- **Where v2 is heading.** Both prototypes are measured here to show what the next API steps would cost.
  - The per-type broker's Lean configuration publishes to 8 subscribers in about half v1.0's instructions. It also
    drops the thread-local storage requirement and is 24 bytes smaller than v1.0.
  - Its Default configuration costs slightly more than the v2 header's default. It adds safe teardown during
    concurrent delivery (known issue K3) and a re-check after `filter()`, which costs 2 instructions per subscriber.
  - Static wiring costs the same as hand-written direct calls: about 6 times fewer instructions than v1.0 for
    8 subscribers, and 12 bytes of code.

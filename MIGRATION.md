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

---

## Behavioral Changes

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

On GCC/Clang, the layout hash captures per-member offset and size via structured bindings (Boost.PFR-style), recursively fingerprinting nested structs and arrays. On MSVC, only the fingerprint (sizeof+alignof+arity) is available due to a compiler bug with structured bindings in template specializations. Full MSVC support is planned for C++26 reflection.

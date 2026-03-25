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

### `publishCanceled_` is now `std::atomic<bool>`

The cancel flag is now atomic, eliminating data races when cancellation occurs across threads.

**Action:** No code change needed.

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

---

## IPC Design Notes

### Endianness

Sub0Pub does **not** perform per-message byte-swapping. All peers on a given IPC channel must share the same byte order. This is by design -- runtime endianness conversion would contradict the library's zero-overhead principle. For mixed-architecture deployments, a connection-time layout verification handshake is planned for a future phase, with full type introspection via [Sub0Reflect](https://github.com/CraigHutchinson/Sub0Reflect).

### Type Layout

The serialization protocol transmits raw bytes (`reinterpret_cast` of the data struct). Both peers must have identical struct layout (size, alignment, member order). There is currently no verification of this at connection time. Future Sub0Reflect integration will enable declaring and checking type layouts across the wire.

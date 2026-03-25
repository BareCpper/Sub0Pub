# Sub0Pub

> Sub0Pub was originally written by hand in 2018 as a spare-time project exploring type-safe messaging in C++. In the current age of AI, we are leveraging it to accelerate development -- driving the concept to a more mature, production-ready level far faster than solo effort allows. This v2 branch is **not yet field-tested**, but it now has a full test suite, CI pipeline, and performance benchmarks, and will be integrated into future products.

**Zero-overhead, type-safe, auto-wiring publish-subscribe for C++**

A header-only messaging library that uses C++ template specialization to route signals at compile time. No `connect()` calls, no signal objects, no MOC toolchain, no allocations. Just inherit, publish, and receive.

Built for **embedded systems**, **game loops**, **desktop applications**, and **distributed IPC**.

```cpp
#include "sub0pub/sub0pub.hpp"

// Publish: inherit and call publish()
class Sensor : public sub0::Publish<float> {
public:
    void sample() { sub0::publish(this, 3.14f); }
};

// Subscribe: inherit and implement receive()
class Display : public sub0::Subscribe<float> {
    void receive(const float& value) override {
        // Automatically called when any Publish<float> fires
    }
};

int main() {
    Sensor sensor;
    Display display;   // Auto-wired on construction
    sensor.sample();   // display.receive(3.14f) called
}
```

---

## Why Sub0Pub?

### What It Does Well

- **Zero-friction wiring** -- Publishers and subscribers connect automatically on construction via the MonoState Broker pattern. No registry, no `connect()`, no boilerplate.
- **Compile-time type routing** -- Message dispatch is resolved entirely through template specialization. No `std::any`, no `void*`, no `dynamic_cast`, no runtime type lookups.
- **Zero allocation** -- No `shared_ptr`, no heap allocation in the hot path. Fixed-size subscription tables live in static storage.
- **Header-only** -- Single file (`include/sub0pub/sub0pub.hpp`), drop into any project, link with `Sub0Pub::Sub0Pub` via CMake.
- **Multi-type subscription** -- `SubscribeAll<A, B, C>` or `SubscribeAll<std::tuple<A, B>>` to subscribe to many types in one class.
- **Built-in IPC serialization** -- `StreamSerializer` / `StreamDeserializer` with a composable binary protocol (`BinaryWriter<Prefix, Header, Postfix>`) for inter-process and network messaging out of the box.
- **Publish cancellation** -- Subscribers can call `cancel()` from within `receive()` to halt further delivery on the current publish cycle.
- **Message filtering** -- Optional `filter(const Data&)` override for per-subscriber message selection at zero cost when unused.

### How It Compares

| Feature | Sub0Pub | Boost.Signals2 | Qt Signals | entt |
|---------|---------|----------------|------------|------|
| Header-only | Yes | Yes | No (MOC) | Yes |
| Zero allocation | Yes | No | No | Partial |
| Compile-time routing | Yes | No | No | No |
| Auto-wiring | Yes | No | No | No |
| Built-in IPC | Yes | No | No | No |

### Current Status

> **Status: v2.0.0-alpha** -- See [PROJECT_STATUS.md](PROJECT_STATUS.md) for the full discipline review.

| Area | Status |
|------|--------|
| Test suite (doctest) | Done -- core pub/sub, cancellation, SubscribeAll, ordering |
| CI/CD (GitHub Actions) | Done -- Linux GCC/Clang, Windows MSVC, macOS |
| Performance benchmarks (nanobench) | Done -- system info capture, 8 scenarios |
| Pre-push test hook | Done -- blocks push on test failure |
| Atomic publish cancellation | Done -- `std::atomic<bool>` |
| Configurable subscriber limit | Done -- `SUB0PUB_MAX_SUBSCRIPTIONS` |
| Compile-time type IDs for IPC | Done -- `utility::typeHash<T>()` |
| Subscription order preservation | Done -- `std::move` replaces swap-remove |
| Broker hidden from public API | Done -- moved to `sub0::detail` |
| Optional thread safety | Done -- `SUB0PUB_THREAD_SAFE` mutex guard |
| Struct-layout fingerprinting | Done -- `makeLayout<T>()` automatic via structured bindings |

### Design Decisions

**Endianness: conformance, not conversion.** Sub0Pub does not perform per-message byte-swapping. All peers on a given IPC channel are expected to share the same byte order. This is a deliberate zero-overhead choice -- runtime endianness conversion on every message would violate the library's core principle.

**Automatic layout verification.** `makeLayout<T>()` produces a `TypeLayout` containing sizeof, alignof, arity, array extent info, and a per-member layout hash -- all automatically via C++17 structured bindings (Boost.PFR-style). No macros, no member lists. On MSVC, per-member decomposition is deferred to C++26 reflection; the fingerprint (sizeof+alignof+arity) still catches most layout mismatches. Full type-member introspection is planned via [Sub0Reflect](https://github.com/CraigHutchinson/Sub0Reflect).

### Known Remaining Limitations

| Issue | Severity | Plan |
|-------|----------|------|
| **Cross-module isolation** -- MonoState `static` state is per-DLL | Medium | Document and provide explicit instantiation pattern |
| **No CRC/checksum** -- only magic prefix + postfix for framing | Low | Add optional integrity check to protocol |
| **Type hash not stable across compilers** -- `typeHash<T>()` uses `__PRETTY_FUNCTION__`/`__FUNCSIG__` | Medium | Use `SUB0PUB_TYPEIDNAME` for cross-compiler IPC |
| **MSVC layout hash limited** -- structured binding bug prevents per-member decomposition | Low | Awaiting C++26 `std::meta::reflect` |

### Roadmap

**Phase 1 -- Foundation:** ~~Fix CMake, add tests, CI, atomic cancellation.~~ Done.

**Phase 2 -- Safety:** ~~Type-ID fix, configurable limits, ordered removal, thread-safe option.~~ Done.

**Phase 3 -- Polish:** ~~Serialization round-trip tests, cross-platform examples, layout fingerprinting.~~ Done.

**Phase 4 -- IPC Hardening:** Optional CRC/checksum protocol layer, connection-time layout verification handshake, [Sub0Reflect](https://github.com/CraigHutchinson/Sub0Reflect) integration for deeper type introspection.

---

## Performance

Benchmarks are built alongside tests and capture system info automatically. Run with:

```bash
./build/tests/Release/Sub0Pub_Bench   # Windows
./build/tests/Sub0Pub_Bench           # Linux/macOS
```

**Reference results:**

> Intel Core Ultra 9 275HX, 24 threads, 31 GB RAM, MSVC 1950, Release build

| Benchmark | ns/op | ops/s |
|-----------|------:|------:|
| Publish (1 subscriber) | 4.2 | 238M |
| Publish (4 subscribers) | 8.3 | 120M |
| Publish (8 subscribers) | 15.0 | 67M |
| Filtered publish (pass) | 4.3 | 230M |
| Filtered publish (reject) | 4.0 | 249M |
| Multi-type dispatch | 4.3 | 233M |
| Subscribe/unsubscribe churn | 3.2 | 315M |
| Empty publish (0 subscribers) | 1.9 | 515M |

Publish cost scales linearly with subscriber count at ~1.5ns per additional subscriber. The ~1.6ns fixed overhead vs v1 is from the snapshot-copy approach that prevents mutex deadlock on re-entrant publish — a correctness trade-off. Multi-type dispatch has zero overhead compared to single-type.

---

## Getting Started

### Requirements

- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.21+

### Build & Test

```bash
cmake --preset default            # Configure
cmake --build --preset default    # Build
ctest --preset default            # Run tests
```

### Install (system-wide)

```bash
cmake --build --preset default --target install
```

### Use in Your Project

```cmake
find_package(Sub0Pub REQUIRED)
target_link_libraries(MyApp PRIVATE Sub0Pub::Sub0Pub)
```

Or add as a subdirectory:

```cmake
add_subdirectory(Sub0Pub)
target_link_libraries(MyApp PRIVATE Sub0Pub::Sub0Pub)
```

---

## Examples

### Minimal Publish/Subscribe

```cpp
class PubInt : public sub0::Publish<uint32_t> {
public:
    void doIt() { sub0::publish(this, 42U); }
};

class SubInt : public sub0::Subscribe<uint32_t> {
    void receive(const uint32_t& value) override { total += value; }
};
```

### Multi-Type Subscribe

```cpp
class Listener : public sub0::SubscribeAll<float, int, std::string> {
    void receive(const float& f) override { /* ... */ }
    void receive(const int& i) override { /* ... */ }
    void receive(const std::string& s) override { /* ... */ }
};
```

### Message Filtering

```cpp
class EvenOnly : public sub0::Subscribe<int> {
    void receive(const int& value) noexcept override { /* handle even values */ }
    bool filter(const int& value) noexcept override { return (value % 2) == 0; }
};
```

### Cross-Module / IPC Serialization

```cpp
// Serialize published signals to a stream
class Recorder : public sub0::StreamSerializer<>
               , public sub0::ForwardSubscribe<float, Recorder>
               , public sub0::ForwardSubscribe<int, Recorder> {
public:
    Recorder(sub0::OStream& out) : sub0::StreamSerializer<>(out) {}
};

// Deserialize and re-publish from a stream
class Player : public sub0::StreamDeserializer<>
             , public sub0::ForwardPublish<float, Player>
             , public sub0::ForwardPublish<int, Player> {
public:
    Player(sub0::IStream& in) : sub0::StreamDeserializer<>(in) {}
};
```

---

## Configuration

Compile-time feature flags (define before including the header):

| Flag | Default | Description |
|------|---------|-------------|
| `SUB0PUB_TRACE` | `false` | Enable event trace logging to `std::cout` |
| `SUB0PUB_ASSERT` | `true` | Enable assertion checks |
| `SUB0PUB_STD` | `false` | Use `std::ostream`/`std::istream` instead of lightweight internal stream types |
| `SUB0PUB_TYPEIDNAME` | `false` | Enable user-defined type IDs and names for IPC |
| `SUB0PUB_THREAD_SAFE` | `false` | Mutex guard for multi-threaded pub/sub |
| `SUB0PUB_REENTRANT_SAFE` | `true` | Snapshot subscribers before dispatch for re-entrant safety. Adds ~1.5ns overhead per publish. Set `false` if you guarantee no subscriber will publish the same type from within `receive()` |
| `SUB0PUB_MAX_SUBSCRIPTIONS` | `8` | Fixed subscription table size per `Broker<T>` |

---

## License

[MIT License](LICENSE.md) -- Copyright (c) 2018 Craig Hutchinson

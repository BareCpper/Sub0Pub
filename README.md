# Sub0Pub

> **v2 is in development on the [`v2` branch](https://github.com/BareCpper/Sub0Pub/tree/v2)** -- featuring a full test suite, GitHub Actions CI, performance benchmarks, atomic cancellation, and API cleanup. See [MIGRATION.md](https://github.com/BareCpper/Sub0Pub/blob/v2/MIGRATION.md) for what's changing.

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

### Known Limitations & Current Status

> **Status: Alpha (v0.1.2)** -- See [PROJECT_STATUS.md](PROJECT_STATUS.md) for the full review.

This project is under active development. The following are known issues with planned mitigations:

| Issue | Severity | Mitigation Plan |
|-------|----------|-----------------|
| **No thread safety** -- Shared static state has no synchronization; multi-threaded use is currently UB | Critical | Phase 2: Add `std::shared_mutex` or document single-threaded contract |
| **No unit tests** -- Zero automated test coverage | High | Phase 1: Integrate test framework, write core + cancellation + serialization tests |
| **CI/CD non-functional** -- Travis config is a skeleton | High | Phase 1: GitHub Actions with GCC/Clang/MSVC matrix + sanitizers |
| **Fixed subscriber limit** -- `cMaxSubscriptions = 8`, silent assert on overflow | Medium | Phase 2: Make configurable via template parameter or `#define` override |
| **IPC type-ID collision** -- All types default to ID `12345` without `SUB0PUB_TYPEIDNAME` | High | Phase 2: Compile-time type hash fallback or `static_assert` |
| **Cross-module isolation** -- MonoState `static` state is per-DLL | Medium | Phase 2: Document and provide `SUB0_BROKERSTATE` macro |
| **C++ standard** -- Claims C++11 but requires C++17 | Low | Phase 1: Update to `cxx_std_17` |

### Roadmap

**Phase 1 -- Foundation:** Fix CMake bugs, add test suite, set up GitHub Actions CI, make `publishCanceled_` atomic.

**Phase 2 -- Safety:** Add Broker synchronization, fix IPC type-ID fallback, make subscriber limit configurable, preserve subscription ordering on removal.

**Phase 3 -- Polish:** API documentation, endianness handling for cross-architecture IPC, CRC/checksum option, consolidate publish conventions, move `Broker` to `sub0::detail`.

---

## Getting Started

### Requirements

- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.7.1+

### Build

```bash
# All platforms
./configure && cmake --build ./build

# Windows (MinGW / Git Bash)
./configure -G "Unix Makefiles" && make -C ./build -j && make -C ./build test
```

### Install (system-wide)

```bash
cmake --install ./build
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
| `SUB0_EXPERIMENTAL` | `false` | Enable experimental features (may be removed) |

---

## License

[MIT License](LICENSE.md) -- Copyright (c) 2018 Craig Hutchinson

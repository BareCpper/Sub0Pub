# Sub0Pub Project Status Review

**Date:** 2026-03-25 | **Version:** 0.1.2 | **License:** MIT

---

## Executive Summary

Sub0Pub is a header-only C++ type-based publish-subscribe messaging library targeting embedded, desktop, games, and distributed systems. The core design is strong: compile-time type safety, zero-allocation MonoState brokering, and a composable IPC serialization layer. However, the project is in an **early alpha** state with critical gaps in testing, thread safety, CI/CD, and documentation that must be addressed before production adoption.

---

## Discipline Reviews

### 1. C++ Architecture & Design

| Aspect | Rating |
|--------|--------|
| Core pattern (MonoState Broker) | Strong |
| Type safety | Excellent |
| IPC composability | Strong |
| CRTP forwarding adapters | Well-designed |
| Cross-module support | Incomplete |

**Strengths**
- Template-parameterized `Broker<Data>` / `Publish<Data>` / `Subscribe<Data>` enforces message types at compile time with no `std::any`, `void*`, or `dynamic_cast` in the hot path.
- MonoState over Singleton avoids global-object-order-of-initialization problems — a pragmatic choice for embedded targets.
- `BinaryWriter`/`BinaryReader` parameterized by `<Prefix_t, Header_t, Postfix_t>` allows compile-time protocol customization with zero virtual dispatch.
- `SubscribeAll<tuple<...>>` tuple flattening via `decltype(std::tuple_cat(...))` is elegant.
- `ForwardSubscribe`/`ForwardPublish` CRTP adapters with `detected_or_t` SFINAE resolve ambiguity cleanly.

**Weaknesses**
- MonoState `static State` breaks across DLL/shared-object boundaries — each module gets its own copy.
- Fixed `cMaxSubscriptions = 8U` is not user-configurable per data-type; exceeding it silently asserts.
- Code claims C++11 but requires C++17 (`inline static`, `if constexpr`, `std::void_t`, `std::is_same_v`).
- `publishCanceled_` semantics across `Broker` instances are fragile and non-obvious.
- `DefaultSerialisation::Header` falls back to hardcoded typeId `12345` when `SUB0PUB_TYPEIDNAME` is disabled.
- `unsubscribe()` swap-with-last silently reorders the subscription table.

---

### 2. Build System & CI/CD

| Aspect | Rating |
|--------|--------|
| CMake library target | Good |
| Package config / install | Good |
| CI/CD pipeline | Non-functional |
| Cross-platform support | Partial |

**Strengths**
- Clean INTERFACE library with namespaced alias (`Sub0Pub::Sub0Pub`) and proper generator expressions.
- Correct subproject detection and gated install logic.
- Working `CMakePackageConfigHelpers` for consumer `find_package()` support.

**Weaknesses**
- **Bug:** `NOT_SUBPROJECT` (line 36) should be `NOT IS_SUBPROJECT` — test block never executes.
- **Bug:** `add_subdirectory(projects)` references a directory that does not exist.
- `.travis.yml` is a skeleton with no `script:` step — effectively non-functional.
- No GitHub Actions workflow; no modern CI.
- `cxx_std_11` declared but C++17 features used throughout.
- `include(GNUInstallDirs)` is commented out but `CMAKE_INSTALL_INCLUDEDIR`/`CMAKE_INSTALL_LIBDIR` are used.
- No compiler warning flags (`-Wall -Wextra` / `/W4`) configured.
- `cross_module` example uses `__declspec(dllexport)` with no cross-platform guards.

---

### 3. Testing & Quality Assurance

| Aspect | Rating |
|--------|--------|
| Unit tests | None |
| Integration tests | Informal (examples only) |
| Static analysis | None |
| Sanitizers | None |
| Code health | Needs attention |

**Strengths**
- Build system has placeholder hooks for GoogleTest, coverage, and Valgrind (intent documented).
- Cross-module example exercises DLL boundary behavior.
- Header-only design simplifies eventual test harness setup.

**Weaknesses**
- `tests/CMakeLists.txt` contains only `# TODO: unit tests!` — zero test coverage.
- 30+ `@todo` comments with no external tracking or severity classification.
- Mixed indentation (tabs/spaces), `#if 0` dead code blocks.
- Typos in identifiers (`dataBufferRegistery_`, `dataBffer`).
- No sanitizer builds (ASan, TSan, UBSan) configured.
- Publish cancellation (most recently changed code) has no test coverage.

---

### 4. API Design & Usability

| Aspect | Rating |
|--------|--------|
| Core API ergonomics | Elegant |
| Discoverability | Poor |
| Documentation | Critically lacking |
| Competitive positioning | Promising |

**Strengths**
- Zero-friction wiring: publishers and subscribers connect automatically on construction — no `connect()` or registry to pass around.
- Beats Boost.Signals2 (explicit signal objects) and Qt (MOC dependency) on setup cost.
- Allocator-free, compile-time routing outperforms entt's `dispatcher` (runtime `type_id`) and Boost.Signals2 (`shared_ptr` overhead).
- `filter()` hook with default `true` is opt-in with zero cost if unused.
- `SubscribeAll` + tuple packs for concise multi-type subscription.

**Weaknesses**
- README is 17 lines with no API reference, usage guide, or design rationale.
- `cancel()` free function takes an unused `data` parameter; undocumented call context.
- Two publish conventions (`member.publish()` vs `sub0::publish(this, data)`) with no guidance on when to use which.
- `Broker<Data>` is publicly visible but should be an implementation detail.
- `cMaxSubscriptions = 8` limit is undocumented — users hit a silent assert.

---

### 5. Thread Safety & IPC

| Aspect | Rating |
|--------|--------|
| Single-threaded correctness | Good |
| Multi-threaded safety | Unsafe (UB) |
| IPC serialization design | Good |
| IPC robustness | Incomplete |

**Strengths**
- `thread_local threadCurrent_` for cancellation tracking is clean and zero-overhead for single-threaded use.
- `BinaryReader` state machine provides structured framing with `SyncLost` recovery.
- `BufferRegister` binary search on sorted arrays is cache-friendly and allocation-free.

**Weaknesses**
- **Data race (UB):** `subscribe()`, `unsubscribe()`, and `publish()` all access shared static `state_` without synchronization. No mutexes, no atomics, no locks anywhere.
- **Data race (UB):** `publishCanceled_` is `mutable bool`, not `std::atomic<bool>` — cross-thread cancellation is undefined behavior.
- **Data race:** Swap-remove in `unsubscribe()` can corrupt the iteration in a concurrent `publish()`.
- No endianness handling — `reinterpret_cast` bulk copies break on cross-architecture IPC.
- `typeId` collapses to `12345` for all types without `SUB0PUB_TYPEIDNAME` — IPC dispatch silently broken.
- No CRC/checksum beyond magic prefix and postfix delimiter.

---

## Risk Matrix

| Risk | Severity | Likelihood | Impact |
|------|----------|------------|--------|
| Data races in multi-threaded use | Critical | High | UB, crashes, memory corruption |
| No test coverage | High | Certain | Regressions go undetected |
| CI/CD non-functional | High | Certain | No automated quality gate |
| IPC type-ID collision (12345 default) | High | High | Silent message misrouting |
| 8-subscriber silent limit | Medium | Medium | Assert in debug, UB in release |
| Cross-module MonoState isolation | Medium | Medium | Subscribers invisible across DLLs |
| Missing documentation | Medium | Certain | Adoption barrier |
| C++ standard mismatch | Low | Low | Build failures on C++11/14 compilers |

---

## Mitigation & Improvement Plan

### Phase 1: Foundation (Critical Fixes)

1. **Fix CMake bugs** — correct `NOT_SUBPROJECT` to `NOT IS_SUBPROJECT`, remove/replace `add_subdirectory(projects)`, uncomment `GNUInstallDirs`, raise standard to `cxx_std_17`.
2. **Add basic test suite** — integrate doctest or Catch2, write core pub/sub tests, cancellation tests, serialization round-trip tests.
3. **Set up GitHub Actions CI** — matrix of GCC/Clang/MSVC, build + test + sanitizer (ASan/UBSan) jobs.
4. **Make `publishCanceled_` atomic** — one-line fix to eliminate the most exploitable data race.

### Phase 2: Safety & Robustness

5. **Add synchronization to Broker** — `std::shared_mutex` (read lock for publish, write lock for subscribe/unsubscribe) or document single-threaded-only contract.
6. **Fix typeId fallback** — replace hardcoded `12345` with compile-time type hash or `static_assert` when IPC is used without `SUB0PUB_TYPEIDNAME`.
7. **Make `cMaxSubscriptions` configurable** — template parameter or global `#define` override with clear error messages on overflow.
8. **Preserve subscription order** — replace swap-remove with `std::move` of tail elements.

### Phase 3: Polish & Adoption

9. **Write API documentation** — MonoState lifetime model, thread-safety contract, feature flags, examples for `SubscribeAll`, `ForwardPublish`, serialization.
10. **Move `Broker` to `sub0::detail`** — reduce public API surface.
11. **Add endianness handling** to serialization headers for cross-architecture IPC.
12. **Add CRC/checksum** option to the serialization protocol.
13. **Consolidate publish convention** — document `sub0::publish(this, data)` as canonical, deprecate or protect member `publish()`.
14. **Audit and triage `@todo` comments** — convert correctness-critical items to tracked issues.

---

## Competitive Position

| Feature | Sub0Pub | Boost.Signals2 | Qt Signals | entt |
|---------|---------|----------------|------------|------|
| Header-only | Yes | Yes | No (MOC) | Yes |
| Zero allocation | Yes | No (shared_ptr) | No | Partial |
| Compile-time type routing | Yes | No | No | No |
| Auto-wiring (no connect()) | Yes | No | No | No |
| Thread safe | No* | Yes | Yes (queued) | Partial |
| IPC/Serialization built-in | Yes | No | No | No |
| Documentation | Minimal | Excellent | Excellent | Good |
| Test coverage | None | Excellent | Excellent | Good |

*Thread safety is achievable with the Phase 2 mitigations outlined above.

Sub0Pub occupies a unique niche: **zero-overhead, type-safe, auto-wiring pub/sub with built-in IPC** — a combination no competitor offers. The path to production readiness is clear and achievable.

# Sub0Pub v2 Improvement Plan

**Generated:** 2026-03-25 | **Based on:** 5-discipline code review of v2 branch

---

## Priority 1 — Critical (correctness/safety bugs)

### 1.1 Fix mutex deadlock on re-entrant publish
**Source:** Architecture review
**Issue:** `Broker::publish()` holds `std::mutex` while calling `receive()`. Any subscriber that itself publishes the same type will deadlock.
**Fix:** Snapshot the subscription array under lock into a local stack copy, release the lock, then dispatch. Standard observer pattern.

### 1.2 Fix `publishCanceled_` scope
**Source:** Architecture review
**Issue:** `publishCanceled_` is per-`Broker` instance but `Broker` is MonoState. Two concurrent publishers of the same type race on each other's cancel flag.
**Fix:** Move `publishCanceled_` to `thread_local` alongside `threadCurrent_`, scoped to the active publish invocation.

### 1.3 Validate Prefix magic in BinaryReader
**Source:** IPC review
**Issue:** `getStateStatus(State::Prefix)` always returns `true`. The SUB0 magic is read but never checked — corrupt streams pass silently.
**Fix:** Compare `prefix_.magic == Prefix_t{}.magic`. On mismatch, enter SyncLost.

### 1.4 Add payload-skip for unknown typeIds
**Source:** IPC review
**Issue:** Unknown typeId in `BinaryReader` causes throw/assert with no recovery. A peer adding a new message type permanently kills the reader.
**Fix:** When `find()` returns null, skip `header_.dataBytes` bytes + postfix, return to Prefix state.

### 1.5 Add SyncLost recovery
**Source:** IPC review
**Issue:** `State::SyncLost` is a dead end — reader stalls permanently.
**Fix:** From SyncLost, byte-scan forward for the next valid magic prefix and re-enter Header state.

---

## Priority 2 — High (correctness gaps, API issues)

### 2.1 Make `Publish::publish()` protected
**Source:** API review
**Issue:** Any code holding a `Publish<T>*` can call `publish()` directly, bypassing encapsulation.
**Fix:** Move to `protected:`, add `friend` for `sub0::publish()` free function.

### 2.2 Add `noexcept` to `Publish::publish()` and `cancel()`
**Source:** Architecture + API reviews
**Issue:** `Broker::publish()` is `noexcept` but the public `Publish::publish()` is not. `cancel()` also lacks it.
**Fix:** Add `noexcept` to both, matching style guide hot-path rule.

### 2.3 Fix `sizeOf<T>()` return type
**Source:** Architecture review
**Issue:** Returns `bool` instead of `size_t`. Dead code but a latent bug.
**Fix:** Delete it (no call sites) or fix return type.

### 2.4 Fix `SUB0_E` macro leak
**Source:** Architecture + API reviews
**Issue:** `#define SUB0_E(base, m)` is never `#undef`'d, polluting the global namespace.
**Fix:** `#undef SUB0_E` after use, or move inside `#if !defined(_MSC_VER)` guard.

### 2.5 Add IPC error path tests
**Source:** Test coverage review
**Issue:** No tests for corrupted prefix, unknown typeId, postfix mismatch, partial reads.
**Fix:** Add `test_ipc_errors.cpp` with corruption and version-skew scenarios.

### 2.6 Document typeHash cross-process limitation
**Source:** IPC review
**Issue:** `typeHash<T>()` differs between compilers. No warning for cross-process use.
**Fix:** Add prominent comment/`static_assert` recommending `SUB0PUB_TYPEIDNAME` for cross-process IPC.

---

## Priority 3 — Medium (infrastructure, DX, robustness)

### 3.1 Add sanitizer CI job
**Source:** Build/CI review
**Issue:** No ASan/UBSan coverage despite `reinterpret_cast` serialization and atomics.
**Fix:** Add `ci-asan` preset (Linux, Debug, `-fsanitize=address,undefined`).

### 3.2 Add `.clang-format`
**Source:** Build/CI review
**Issue:** Style guide exists but is not enforced.
**Fix:** Generate `.clang-format` from STYLE_GUIDE.md rules.

### 3.3 Fix pre-push hook robustness
**Source:** Build/CI review
**Issue:** Fails confusingly on fresh clone (no build dir). Opt-in only.
**Fix:** Add `cmake --preset default` fallback. Document in CONTRIBUTING.md.

### 3.4 Add `ForwardSubscribeAll`/`ForwardPublishAll` tests
**Source:** Test coverage review
**Issue:** These variadic forwarding classes are completely untested.
**Fix:** Add round-trip test using the All variants.

### 3.5 Add `SUB0PUB_MAX_SUBSCRIPTIONS` overflow test
**Source:** Test coverage review
**Issue:** No test approaches the subscription limit.
**Fix:** Test at-capacity behavior in a separate TU with reduced limit.

### 3.6 Enable examples in default preset
**Source:** Build/CI review
**Issue:** Examples disabled locally, only built in CI. Breakage not caught locally.
**Fix:** Set `SUB0PUB_BUILD_EXAMPLES=ON` in default preset.

### 3.7 Fix `paddingSize` overflow risk
**Source:** IPC review
**Issue:** `int_least16_t paddingSize` can overflow on large payloads.
**Fix:** Use `int32_t` and loop the discard read.

### 3.8 Add filter() example to README
**Source:** API review
**Issue:** filter() is mentioned but never shown.
**Fix:** Add 6-line example to README.

---

## Priority 4 — Low (polish, naming, minor cleanup)

### 4.1 Rename `SUB0_STRINGIFY` to `SUB0PUB_STRINGIFY`
**Source:** API review — inconsistent with `SUB0PUB_` prefix convention.

### 4.2 Remove `SUB0_EXPERIMENTAL` from README config table
**Source:** API review — macro no longer exists in header.

### 4.3 Fix arity upper bound
**Source:** Architecture review — always use `MaxArity` instead of `min(sizeof(T), MaxArity)`.

### 4.4 Add `[[nodiscard]]` to `makeFingerprint`/`makeLayout`
**Source:** API review — results are useless if discarded.

### 4.5 Annotate `makeLayout` MSVC degradation
**Source:** API review — add a note or warning when per-member hash is unavailable.

---

## Execution Order

**Batch 1 (Critical fixes):** 1.1, 1.2, 1.3, 1.4, 1.5
**Batch 2 (API + noexcept):** 2.1, 2.2, 2.3, 2.4
**Batch 3 (Tests):** 2.5, 3.4, 3.5
**Batch 4 (Infrastructure):** 3.1, 3.2, 3.3, 3.6
**Batch 5 (Polish):** 3.7, 3.8, 4.1-4.5

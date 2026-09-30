# v2 optimization pass

This pass covers the static wiring, runtime broker and concurrency paths, IPC registration/fingerprinting,
public include graph, test build and performance tooling. It is a targeted review with measured follow-up,
not a claim that every workload or toolchain has been optimized.

## Changes and trade-offs

| Area | Diagnosis | Change / decision |
|---|---|---|
| Aggregate fingerprints | Both operands of a conditional expression instantiate recursive arity branches; the apparent binary search visits the entire tree | Select the recursive type with `std::conditional_t` first. Also select only the applicable constructibility trait. Values and the 32-member limit are unchanged. Boundary tests cover 15/16/17/31/32 and array fingerprints. |
| Broker include cost | `<algorithm>` is included only for two pointer-array copies | Use standard `memcpy` for a non-overlapping snapshot; combine domain-close recording with its clearing loop. No hand-written replacement for a general algorithm. |
| Concurrent close | Detached-pointer copies are not consumed by the concurrent path | Record pointers only for the non-concurrent path. Retain locks, atomic ordering, active-dispatch invalidation and waits. |
| Test build | Three executables compile the same doctest implementation | One object target shared by the three executables. Test TUs remain independent; no PCH or unity build that could conceal missing includes or cross-TU defects. |
| IPC capacity | Registration asserts before checking replacement; release insertion can overrun a full array | Add `trySet(header, buffer)`, allow replacement at capacity, and reject new entries before mutation. Existing `set()` asserts on overflow in debug and rejects it safely in release. |
| Compile evidence | Wiring/umbrella workloads do not instantiate aggregate fingerprints | Add a narrow layout profile with distinct 32-member types and recursive array fingerprints. Both revisions receive identical fixture bytes. |

The shared test runner includes doctest only, so suite-specific Sub0Pub macros remain on the actual test TUs.
This removes two runner compilations per clean build; no percentage claim about total parallel build time is made.

## Decisions retained

- Static wiring's folds, capability routing and erased Sink already have equal-work collapse evidence. No new
  runtime dispatch, allocation or registration is added there.
- The broker's small-table search/shift stays as measured. Snapshot lifetime and mutation semantics are retained;
  weakening sequential consistency or dropping callback waits would change the contract.
- `<thread>` is expensive to parse but is required by per-type concurrent configurations even when global defaults
  disable concurrency. Macro-based omission would break opt-in types. A separate concurrency header is a possible
  future API design, not a safe include deletion.
- The umbrella retains compatibility includes. Narrow headers remain the appropriate consumer choice when only
  one area is used. IPC keeps standard binary lookup and insertion algorithms for its sorted fixed-capacity table.
- No modules, PCH, unity builds, type-erased broker core, allocator or scheduler dependency is introduced. Those
  trade portability, consumer integration or runtime cost and need their own equal-work evidence.

Known design limitations in [DESIGN.md](DESIGN.md), including mutual-disconnect deadlock (K4), remain. This pass
also does not establish contended throughput, embedded stack bounds or cross-module ABI support.

## Diagnostic evidence

GCC 13.3, C++23, `-DNDEBUG`, direct include-only preprocessing (`-E -P`):

| Header | Before bytes / lines | After bytes / lines |
|---|---:|---:|
| wiring | 472,131 / 13,275 | 472,131 / 13,275 |
| broker | 1,737,932 / 49,594 | 1,380,314 / 39,071 |
| umbrella | 1,800,266 / 50,897 | 1,800,601 / 50,905 |

This is a structural observation, not a timing benchmark: the narrow broker's preprocessed input falls about
20.6%; the umbrella still includes algorithms for IPC and compatibility. A baseline GCC `-ftime-report` placed
91% of wall time in parsing and 2% in constraint satisfaction for the include-only broker probe. These single
observations identify candidates; repeated consumer A/B measurements are needed for performance claims.

See [COMPILE_TIME.md](COMPILE_TIME.md) for the measurement method, historical C++23 migration warning and captures.

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

## Repeated measurements and validation

The [same-mode A/B capture](perf/compile-time/optimization.md) compares `cf0fd365` with `17047154`,
eight translation units and five paired samples per profile. Layout compilation fell 45.92%, with
non-overlapping ranges. The broker median fell 23.72%, but overlapping ranges and baseline variability
limit confidence in the precise magnitude. Umbrella compilation was unchanged (-0.14%). Wiring's -13.48%
has overlapping ranges and no changed include/workload path; it is not attributed to these changes.
Raw samples, compiler/host details and fixture/harness hashes are retained beside the report.

Validation of optimization commit `17047154ae9e2cdd8233b3378ea0c883b3d0cd56`:

- GCC 13.3 Release: all 253 CTest entries passed, including header isolation, installed consumers, configuration,
  concurrency, collapse behavior and benchmark harness contracts.
- Targeted capacity tests passed ASan/UBSan in debug (61 assertions) and release (63 assertions). Local leak
  detection was disabled because the sandbox blocked LeakSanitizer's `/proc` access.
- All six runtime benchmark executables completed locally. Their wall-clock output is a smoke check, not a
  before/after speedup claim; local Callgrind was unavailable.
- The local final-image run produced 252 matching behavior records. The budget command failed because
  instruction counts were unavailable and some image-size deltas exceeded recorded budgets. All 18 filtering
  records were identical to the parent on this host (checksums, sections, paths and dependencies), establishing
  that the observed filtering overruns predated this pass. No budget was widened.
- [CI run 36731229402](https://github.com/CraigHutchinson/Sub0Pub/actions/runs/36731229402) passed GCC, Clang,
  AppleClang, MSVC, debug/release ASan/UBSan, TSan and the full collapse-evidence job with its configured toolchains.
  This supplies the complete gate that the local profiler environment could not provide.
- [Compile A/B CI run 36731229252](https://github.com/CraigHutchinson/Sub0Pub/actions/runs/36731229252) passed.

No runtime latency reduction is claimed. The demonstrated improvement is compilation work, removal of repeated
test-runner compilation, and bounded IPC registration. Contended runtime profiling and embedded stack measurement
remain separate work, as recorded in the design limitations.

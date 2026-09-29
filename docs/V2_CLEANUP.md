# v2 release cleanup (staged after core examples)

Goal: a focused v2 source tree containing the supported library, useful examples, public regression tests,
reproducible benchmarks and a concise migration guide. Preserve provenance in git; do not rewrite the shared
`v2` branch or erase the `v1.0` baseline. A clean source tree does not require an orphan history.

## Compatibility boundary

Examples teach the current API without a versioned directory. The runtime broker (`Publish`, `Subscribe`,
`SubscribeAll`) is a current dynamic path, not a v1 shim. There is no separate v1 API adapter today.
If an old-contract adapter is introduced, keep its samples under `examples/compatibility/v1/` and record
its replacement, consumers still requiring it and explicit removal criteria. That layer is removable
migration debt; the supported core paths must not depend on it. The historical v1 benchmark remains
comparison evidence rather than a compatibility API.

The umbrella header remains a supported entry point. Its retained transitive standard-library includes
are a distinct source-compatibility concession; audit and document consumer migration before removing
those includes, rather than deleting or mislabelling the whole umbrella.

## Stage 1 — core coverage

- [x] Add checked static, dynamic, mixed and transport examples using only the public API.
- [x] Add mixed-path occupancy, churn and capacity-rejection comparison sources.
- [ ] Obtain green GCC, Clang, Windows/macOS and sanitizer CI for this set.
- [ ] Capture the refreshed migration comparison and confirm all public cost budgets pass.

## Stage 2 — removal changeset

Apply only after Stage 1 is green. Keep this separate from functional optimizations so changes can be reviewed independently.

| Remove from final source tree | Dependencies to update first | Proof to retain |
|---|---|---|
| `tests/design/broker_config/` (frozen broker prototype) | tests/CMakeLists, benchmark runner's prototype table, footprint tool imports, collapse prototype include paths and `sub0x_dynamic*` cases | Public `tests/config`, v1 tag comparison, latest public measurements |
| `tests/design/quiescence/` (alternative lifetime experiments) | tests/CMakeLists; performance/design links | Public concurrent teardown, mutual-wait limitation, ASan/TSan tests; retain any unmatched regression first |
| `tests/collapse/sandbox/`, sandbox unit and compile-fail tests | Port any unmatched checks to `tests/wiring`; remove corresponding CMake targets | Public wiring guarantees and misuse diagnostics |
| `sub0x_*` and `sub0pub_spike` collapse variants; `sub0pub_variants/` | Discovery/scoring scripts, prototype reference selectors, feature probes used only by removed variants | Every `sub0_*`/`sub0pub_virtual*` case, hand-written reference and all 510 public budget rows |
| `tests/footprint/spike/` and abandoned language experiments | Footprint/report scripts and links | Public host/Cortex-M measurements, documented C++17 baseline |
| Phase 0/1 generated reports, prototype scorecards, resolved review-response notes | Replace live links with current design/migration/evidence summaries; identify historical commit for archaeology | One current decision record, public report + raw data, v1 comparison and reproducible commands |
| Legacy `configure` / `cmake-install.sh` if superseded | Check downstream use and documented preset/install commands | Fresh checkout configure/build/install and a find_package consumer test |

Retain `examples/cross_module/`: it represents an unresolved shared-library use case, not a superseded
experiment. Its disabled build is a support gap to close; separate-translation-unit tests do not prove
cross-DLL state sharing or safe module unloading. See its [status](../examples/cross_module/README.md).

Do not remove the v1 comparison harness, current umbrella entry header, known-limitations documentation,
hand-written benchmark references or negative lifetime tests as “legacy”. They prove behavior users rely on.
Do not silently increase a budget to make cleanup pass.

## Stage 3 — acceptance

1. Fresh checkout: configure/build/test Debug and Release, install, then compile an external `find_package(Sub0Pub)` consumer.
2. Compare public behavior and generated code before/after removal with identical compiler versions/flags.
3. Verify every supported use-case row in the coverage ledger still has a public correctness test and a runnable recipe
   (deeper contract cases may be demonstrated by the linked tests). Keep performance gaps explicitly open.
4. Run link/reference checks and `rg` for deleted paths/prototype names; explain historical mentions that remain.
5. Green cross-platform and sanitizer CI, unchanged public budget coverage, no prototype-only required build targets.
6. Squash the cleanup PR into `v2` if a single reviewable release-preparation commit is desired. Preserve tagged v1 history.

Contention/teardown-tail timing and embedded stack measurement remain distinct evidence follow-ups; MSVC
final-link evidence ([design/MSVC_VERIFICATION.md](design/MSVC_VERIFICATION.md)) must be regenerated, not dropped. Removing experiments must not make those gaps disappear from the release notes.

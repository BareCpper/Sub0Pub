# v2 release cleanup (staged after core examples)

Goal: a focused v2 source tree containing the supported library, useful examples, public regression tests,
reproducible benchmarks and a concise migration guide. Preserve provenance in git; do not rewrite the shared
`v2` branch or erase the `v1.0` baseline. A clean source tree does not require an orphan history.

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
| `examples/cross_module/` (disabled, unsupported MSVC DLL example) | examples/CMakeLists and any README links | Explicit unsupported cross-DLL contract; supported separate-TU test |
| Phase 0/1 generated reports, prototype scorecards, resolved review-response notes | Replace live links with current design/migration/evidence summaries; identify historical commit for archaeology | One current decision record, public report + raw data, v1 comparison and reproducible commands |
| Legacy `configure` / `cmake-install.sh` if superseded | Check downstream use and documented preset/install commands | Fresh checkout configure/build/install and a find_package consumer test |

Do not remove the v1 comparison harness, compatibility umbrella header, known-limitations documentation,
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

MSVC final-link evidence, contention/teardown-tail timing and embedded stack measurement remain distinct evidence
follow-ups. Removing experiments must not make those gaps disappear from the release notes.

# MSVC verification record

Scope: the behaviour and compile-fail suite on MSVC (stages 1-3 of the MSVC item in
[COLLAPSE_EVIDENCE.md](COLLAPSE_EVIDENCE.md)). Codegen evidence (`dumpbin`) is recorded separately once done.

## Run 2026-09-29 — `ci-msvc` preset, unmodified sources

| | |
|---|---|
| Commit | `8f022ab` (`claude/sub0pub-issue9-collapse`, after `v2`) |
| Compiler | MSVC 19.51.36246 (VS 18 Community, toolset 14.51.36231), x64 |
| Configure | `cmake --preset ci-msvc` (tests + examples ON, Visual Studio generator, Release) |
| Build | success, 0 errors, 492 warnings (all `/W4`, see below) |
| ctest | **300 / 300 passed**, 0 failed, 0 skipped (26 s) |
| doctest (`Sub0Pub_Tests`) | 47 / 47 test cases, **149 / 149 assertions** |

The 300 tests include every `Collapse_*` behaviour case (each variant, observable and removable form),
`Sub0Pub_CollapseSandboxTests`, the `Sub0Pub_Collapse_cf_*` and `Sub0Pub_Sub0x_cf_*` compile-fail probes
(including `cf_static_wiring_element`, which passes on MSVC's C2975 wording), and the evidence gate self-tests.

### Warnings at `/W4`

| Code | Count | Where | Meaning |
|---|---|---|---|
| C4100 | 284 | `sub0pub.hpp` (264), `sub0pub_spike.hpp`, tests | unreferenced parameter |
| C4267 | 146 | `sub0pub.hpp` (140), `sub0pub_spike.hpp` | `size_t` narrowed |
| C4244 | 36 | `sub0pub.hpp` | conversion, possible data loss |
| C5285 | 18 | vendored `doctest.h` | specialises `std::tuple` (vendor code) |
| C4702 | 8 | `sub0x_static.hpp` | unreachable code |

Counts are MSBuild output lines (each is repeated per translation unit that includes the header), not distinct
sites. The library header itself produces most of them. They do not fail the build (no `/WX`), and are not
fixed here.

### Not covered by this run
- Debug configuration and sanitizers (`ci-asan`/`ci-tsan` are non-Windows presets).
- `examples/cross_module` is disabled on MSVC (`thread_local` members of `Broker<>` cannot be `dllexport`ed).
- Per-member layout hash is unavailable on MSVC (fingerprint only); `test_fingerprint.cpp` skips those assertions.
- Codegen evidence (instruction counts, RAM, static initialisers): see the `dumpbin` work.

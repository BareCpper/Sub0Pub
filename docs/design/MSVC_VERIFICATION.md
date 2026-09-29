# MSVC verification record

Two parts: the behaviour and compile-fail suite (ctest), and the collapse codegen evidence from the final PE image
(`dumpbin`, the MSVC item in [COLLAPSE_EVIDENCE.md](COLLAPSE_EVIDENCE.md)).

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

### Not covered by the ctest run
- Debug configuration and sanitizers (`ci-asan`/`ci-tsan` are non-Windows presets).
- `examples/cross_module` is disabled on MSVC (`thread_local` members of `Broker<>` cannot be `dllexport`ed).
- Per-member layout hash is unavailable on MSVC (fingerprint only); `test_fingerprint.cpp` skips those assertions.

## Collapse codegen evidence (`dumpbin`)

`python tests/collapse/collapse_evidence.py --build msvc-O2` (and `msvc-O2-lto` for the cross-file case) runs from
any prompt: when `cl` is not on PATH the tool asks `vswhere` for the newest Visual Studio (preview channels included)
and imports its x64 environment. Stored results: [phase1-msvc-2026-09.md](../perf/collapse/phase1-msvc-2026-09.md)
and [phase1-msvc-lto-2026-09.md](../perf/collapse/phase1-msvc-lto-2026-09.md) (JSON beside them).

**How it measures.** `cl /O2 /GS- /Gy /Gw /EHsc`, linked with `/OPT:REF /OPT:NOICF /INCREMENTAL:NO /DEBUG /MAP`.
The publish path is `dumpbin /disasm` of `collapse_publish` plus the user-object functions it reaches; a function the
linker map attributes to a library (CRT) object, or an import, is external, like a PLT stub (calls and tail jumps
alike). Section sizes come from the map, by section name as GNU `size` counts an ELF: data = `.data*`/`.tls*`,
bss = `.bss`, text = everything else, including the unwind tables `.pdata`/`.xdata` (an ELF's `.eh_frame` is text);
the debug directory `.rdata$zzzdbg` is left out because its size follows the PDB file name. init = `.CRT$XC*`
pointers. Retained Sub0Pub code comes from the map's symbols; the map's `f` flag says which are functions.
`/GS-` matches the ELF builds (no stack cookie); `/OPT:NOICF` matches `--gc-sections`, which does not fold.

**Not measured on Windows: callgrind instruction counts.** The publish/setup/teardown instruction criteria are `-`
for MSVC; the static publish-path count and the checksum are what MSVC is judged on.

### Result (2026-09-29, MSVC 19.51.36246, 19 cases, both forms)

334 links (`msvc-O2` and `msvc-O2-lto`): 0 build errors, 0 behaviour mismatches, 0 skipped variants
(deducing-this and `std::expected` variants build).

| Verdict against the equal-work reference (all measurable criteria except instruction counts) | MSVC | GCC 13.3 | Clang 18 |
|---|---|---|---|
| B1 `sub0x_b1*` (runtime binding) | 26 / 30 | 27 / 30 | 25 / 30 |
| B2 `sub0x_b2*` (static wiring) | 30 / 32 | 31 / 32 | 31 / 32 |
| B3 `sub0x_b3*` (`Sink` erasure) | 20 / 20 | 20 / 20 | 11 / 20 |
| Pattern A `sub0pub_*` (today's API) | 0 / 46 | 0 / 46 | – |

Pattern A fails on MSVC exactly as on GCC (extra path, RAM, TLS and retained code); the collapse claims for B1-B3
hold on MSVC to within the same residuals GCC and Clang show.

### Findings
1. **The MSVC inliner did not collapse a 32-receiver static wiring.** `many_receivers/sub0x_b2_static` compiled
   `StaticWiring::publish` out of line: path 169 against 69 for handwritten. Marking the delivery functions
   `__forceinline` (`SUB0X_INLINE` in `sandbox/sub0x_static.hpp`; plain `inline` on every other compiler, so
   GCC/Clang/Cortex-M33 code is unchanged) brings it to 71 (+2, inside tolerance), and B1's publish path to that of
   its runtime reference (197, +0). This is a sandbox change; the public `sub0pub.hpp` is untouched.
2. **Every remaining B1/B2 failure** (4 of 30 B1, 2 of 32 B2, 2 LTCG B2):
   - `many_receivers` B1 (`sub0x_b1_wire` vs `handwritten_runtime`), both forms: "no Sub0Pub retained". The
     path is equal, but the 32-binding `Wiring` keeps out-of-line sub0x code (1104 B observable, 368 B removable)
     and text is +304 B. GCC collapses it fully.
   - `many_receivers` B2 observable: the application's own `Sensor<StaticWiring<...>>::send` stays out of line
     (400 B, one direct call, path +2), so text is +48 B and it counts as retained sub0x code; RAM +16 B
     (alignment). GCC inlines `send` into `collapse_publish`. The removable form passes.
   - `nested_publish` B2 observable: RAM +16 B (`.bss`); text and path are equal.
   - `transport_two_links` B1 (`sub0x_b1_wire`): publish path +10 (observable) / +8 (removable).
     `sub0x_b1_wire_typed_links` meets every criterion. GCC fails the same variant on the publish path.
   - LTCG `cross_file` B2, both forms: RAM +8 B (`.bss`); path and text are equal.
3. **MSVC-specific measurement pitfalls, handled in the tool** (each has a `--self-test` case):
   - A variable's decorated name embeds its type, so `Slot<Wiring<...>> bus` would look like retained `sub0x`
     code; for a variable only its own qualified name counts. A function's name also contains the variable marker
     `@@3` when a template argument is `&variable` (`StaticWiring<&relay>`), so functions are told apart by the
     map's `f` flag, not by the name. `$unwind$` records are excluded (their function is reported itself).
   - `.pdata` (12 B per out-of-line function) is unwind data, counted as text, not RAM.
   - `.rdata$zzzdbg` varies with the output file name (±8 B between identical code) and is excluded.
   - Tail jumps to library functions or imports are external direct calls, as `jmp f@plt` is on ELF.
   - `driver.cpp` `opaque()` was a no-op on MSVC (no inline asm); it now round-trips a `volatile`.
4. `handwritten_erased` links the C++ exception runtime (`FindHandler` ...) on MSVC, +12.5 KB text. That is the
   reference's own cost, the same for every variant that erases through it.

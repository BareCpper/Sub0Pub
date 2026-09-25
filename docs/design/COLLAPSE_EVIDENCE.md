# Collapse Evidence: measuring the Sub0 goal (issue #9)

**Status:** Phase 0, the measurement loop, is in place. It records today's gap for pattern A (the current
`sub0pub.hpp` API). Phase 1, the sandbox of candidate coding patterns, is next.
**Gate:** #9 is a v2 acceptance gate. The broker API in [BROKER_CUSTOMISATION.md](BROKER_CUSTOMISATION.md) (#8)
is not frozen until it can select the structure this evidence shows is needed.

The Sub0 intent is **correctness without cost**: wherever an application's topology and behaviour allow
it, the compiler must be able to remove dispatch, registration, storage and context machinery. This
document defines how that is measured. Candidate designs and coding patterns are judged by that
measurement, not argued for.

## The loop

1. **Cases.** `tests/collapse/cases/<case>/` holds one application scenario per directory:
   - `handwritten.cpp` is the equal-work reference, written without Sub0Pub;
   - every other `.cpp` implements the *same behaviour* through a Sub0Pub coding pattern (a *variant*).

   Each variant defines `collapse_setup()`, `collapse_publish(v)` and `collapse_teardown()`
   (`tests/collapse/collapse_case.hpp`). These entry points are the boundary of the measured application
   code; anything inside them may be inlined.
2. **Forms.** Every case is built twice:
   - **observable-work:** receivers change observable state, and the checksum must equal the reference's;
   - **removable-work:** receivers do no observable work, so ideal code removes the machinery around them.

   Argument side effects (`collapse::arg`) are observable in both forms and must survive.
3. **Behaviour check (ctest, every CI compiler including MSVC).** `tests/collapse/CMakeLists.txt` builds
   every variant in both forms and fails if its output differs from the reference.
4. **Evidence (`tests/collapse/collapse_evidence.py`).** For every case, variant, form and named build, it
   links a real executable and reports:

   | Evidence | Source | Criterion against handwritten (same build and form) |
   |---|---|---|
   | behaviour | the executable's checksum | identical |
   | publish / setup / teardown instructions | callgrind client requests in `driver.cpp` | publish within +2; setup and teardown no more |
   | publish path | final-ELF disassembly of `collapse_publish` plus directly reachable functions | static instructions within +2; no extra indirect calls |
   | RAM | final-ELF `.data` + `.bss` | no more |
   | static initialisation | `.init_array` size | no more |
   | retained Sub0Pub code and state | `sub0::` symbols left in the final ELF | none |
   | link dependencies | TLS, `operator delete`, `__cxa_pure_virtual`, atexit | none added |
   | where extra bytes come from | largest symbols added over the reference | reported |

   Behaviour mismatches fail the tool. Criterion verdicts are reported without being enforced, because
   pattern A is expected to fail them. Every link also writes a linker map next to its executable.

Named builds: `gcc-O2` and `clang-O2` (x86-64, run natively and under callgrind), and `cm33-gcc-Os`
(arm-none-eabi GCC 13, Cortex-M33 as on the nRF54 application core, final ELF only). Still to add: MSVC
(ELF-equivalent evidence via `dumpbin`), an LTO on/off pair for the cross-file case, and RISC-V once a
toolchain with libstdc++ is available.

```bash
python3 tests/collapse/collapse_evidence.py [--case one_receiver] [--build cm33-gcc-Os] [--json out.json] > report.md
```

Stored results: [../perf/collapse/](../perf/collapse/). Each phase adds a dated report and JSON file.

## Acceptance cases (issue #9)

| Case | Directory | Status |
|---|---|---|
| Zero static receivers | `zero_receivers` | pattern A measured |
| One concrete receiver | `one_receiver` | pattern A measured |
| Multiple receivers, including repeated types | `multi_receivers` | pattern A measured |
| Default and runtime filters | `filters` | pattern A measured |
| Two independent domains | | Phase 1 (sandbox) |
| Concrete transport endpoint | | Phase 1 (sandbox) |
| Dynamic subscriptions | | Phase 1: regression limits for the runtime path |
| Cross-file application | | Phase 1: receivers in another TU, with and without LTO |

## Phase 0 results: pattern A (today's API)

Full report: [../perf/collapse/phase0-pattern-a-2026-09.md](../perf/collapse/phase0-pattern-a-2026-09.md).
Deltas are against handwritten.

| Case (observable form) | gcc publish instr | clang publish instr | cm33 publish path instr | cm33 text | cm33 RAM | cm33 added deps |
|---|---:|---:|---:|---:|---:|---|
| zero receivers | +10 | +0 | +23 | +720 B | +404 B | TLS, operator delete |
| one receiver | +32 | +60 | +131 | +1540 B | +148 B | operator delete |
| multiple receivers | +99 | +117 | +132 | +1752 B | +432 B | TLS, operator delete |
| default + runtime filter | +103 | +91 | +149 | +1804 B | +420 B | TLS, operator delete |

Every pattern A variant fails the collapse criteria on every build. Behaviour is identical in all cases.

### Findings
1. **The compiler can already collapse part of today's API, but only in the best case.** With every type in
   one TU and internal linkage (anonymous namespace), GCC reduces the removable one-receiver publish to exactly
   the hand-written code (8 instructions), and removes the TLS accesses. Clang does the same for zero receivers.
   It works because the optimiser can see every subscriber type and every use of the broker's state.
   Real applications share message types across TUs with external linkage, so the cross-file case (Phase 1)
   measures the realistic situation.
2. **Publication collapsing is not enough.** Even where publish collapses, the cost of the registry remains:
   - setup: +3 to +38 instructions (registration);
   - teardown: up to +121 (virtual destructors that unsubscribe);
   - RAM: static tables of 72 B per type plus vtables;
   - image: +0.7 to +4.8 KB, from destructors, vtables and typeinfo.

   This is what the review meant by "construction, destruction, RAM, initialisation and retained code, not
   just instructions per publish".
3. **Bare-metal link dependencies.**
   - Pattern A needs a TLS runtime (`__aeabi_read_tp`) wherever the compiler can't prove the publish
     context unused. `tests/collapse/support/bare_metal_tls.cpp` is a link-only stub.
   - Virtual destructors pull `operator delete` and, through it, newlib's `malloc`/`free`/`sbrk` into an image
     that never allocates: 256 B `_malloc_r` + 168 B `_free_r` + support code.
4. **Clang keeps more Sub0Pub code than GCC** (596 B against 257 B retained `sub0::` symbols in the same cases).
   Results must be recorded per compiler.

## Plan (issue #9)
- **Phase 1: sandbox.** A small, realistic application:
  - a sensor publishes to two controllers of the same type and a logger;
  - two independent domains, one transport endpoint;
  - receivers in another TU.

  Candidate coding patterns are implemented side by side as variants in these cases, starting with
  typed bindings at the composition point (pattern B). The recommended hot-loop pattern and its
  ergonomics are chosen from this evidence.
- **Phase 2:** broker specialisation (#8) selects the chosen static structure per message type or domain,
  with runtime subscription kept at the dynamic boundary.
- **Phase 3:** decision record here and in BROKER_CUSTOMISATION.md; compromises go into its known-issues list.

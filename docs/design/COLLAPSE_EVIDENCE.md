# Collapse Evidence: measuring the Sub0 goal (issue #9)

**Status:** Phase 0 (measurement loop) and the first Phase 1 round (sandbox of candidate coding patterns)
are in place. All eight acceptance cases are measured. The **draft recommendation** is in "Phase 1 results".
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

| Case | Directory | Variants measured |
|---|---|---|
| Zero static receivers | `zero_receivers` | A, B1, B2, B3 |
| One concrete receiver | `one_receiver` | A, B1, B2, B3 |
| Multiple receivers, including repeated types | `multi_receivers` | A, B1, B2, B3 |
| Default and runtime filters | `filters` | A, B1, B2, B3 (B variants declare an always-true filter that must disappear) |
| Two independent domains | `two_domains` | B1, B2, #8 prototype `Domain` (A cannot express two sessions of one type) |
| Concrete transport endpoint, egress + ingress | `transport_endpoint` | B1, B2, #8 prototype `Route` (A has no split horizon, so ingress would echo) |
| Dynamic subscriptions | `dynamic_subscriptions` | A, #8 prototype; the reference is a minimal hand-written runtime registry |
| Cross-file application | `cross_file` | A, B1, B2; receivers in another TU with external linkage; LTO on and off |

Variant naming: `sub0pub_virtual` is pattern A (today's API); `sub0x_b1_wire`, `sub0x_b2_static` and
`sub0x_b3_sink` are pattern B (`tests/collapse/sandbox/sub0x_static.hpp`); `sub0x_dynamic*` is the #8
runtime-registry prototype.

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

## Phase 1 results: pattern B (typed bindings at the composition point)

Full report: [../perf/collapse/phase1-patterns-2026-09.md](../perf/collapse/phase1-patterns-2026-09.md).
Every variant of every case behaves identically to its hand-written reference (ctest and checksums).

**Pattern B** (`tests/collapse/sandbox/sub0x_static.hpp`):
- **Receivers are ordinary classes**: a non-virtual `receive(const T&)` for each message type handled, and
  optionally `bool filter(const T&)`. There is no base class, no registry, and no registration at construction.
- **Concrete instances are bound where the application composes itself.** Routing is by capability:
  `publish<T>` calls, in bound order, every bound receiver that has `receive(const T&)`.
- **Message definitions never list receivers.** Two instances of one type are two bindings; two sessions are
  two wirings.

| Form | What the user writes | Where identity is erased |
|---|---|---|
| **B2 static topology** | `using Bus = sub0x::StaticWiring<&controllerA, &controllerB, &logger>;` (static-storage objects); publishers call `Bus::publish(msg)` or are templated on `Out` | never |
| **B1 runtime-bound wiring** | `auto bus = sub0x::wire(controllerA, controllerB, logger);`; publishers are templated on their output (`template<class Out> struct Sensor { const Out& out; ... }`) | never (addresses are runtime values, types are static) |
| **B3 type-erased publisher port** | a non-template publisher holds `sub0x::Sink<Sample>` constructed from a wiring | at the publisher: exactly one indirect call, everything behind it typed |
| Transport endpoint | `sub0x::Forward<Radio>` / `sub0x::StaticForward<&radio>` bound like a receiver; ingress `bus.publishFrom(endpoint, msg)` skips that endpoint | never |

**Against hand-written equal-work code** (deltas; "=" means identical on every criterion):

| Case | B2 static | B1 wire | B3 sink | Pattern A (today) |
|---|---|---|---|---|
| zero / one / multi receivers, filters | **= on all builds** | publish +0 to +9, RAM +0 to +48 B | publish +0 to +21 (one indirect call) | publish +0 to +117 (+0 only where the compiler proves the table empty), teardown up to +121, +0.7 to +4.8 KB |
| two domains | **= on all builds** | publish +0 to +10, RAM +0 to +48 B | not measured | not expressible (#8 prototype: +143 to +169) |
| transport endpoint | **= on all builds** | publish +3 to +4 | not measured | not expressible (#8 prototype: +175 to +209) |
| cross-file, no LTO | **= on all builds** | publish +6 to +7 | not measured | +69 to +95 |
| cross-file, LTO | **= on all builds** | publish = on Clang, +2 to +4 on GCC; setup +0 to +8, RAM +8 to +32 B | not measured | +96 to +115: LTO does not devirtualise the runtime registry |

Dynamic subscriptions (runtime path, regression limits against a minimal hand-written registry): publish
+32.5 (GCC) and +46.3 (Clang) for pattern A, and +46.5 and +55.5 for the #8 prototype. The runtime path
stays supported, at an explicit, measured cost.

### Answers to the three design questions (draft, from evidence)
1. **Where are concrete subscriber instances bound?** At the application's composition point, by the wiring.
   Neither in the message definition, nor at construction of the receiver.
2. **When does their identity become type-erased?** Only at an explicit boundary the user chooses: a
   `Sink<T>` publisher port (one indirect call), or a runtime registry for genuinely dynamic subscribers.
   Otherwise, never.
3. **Can the normal high-level API avoid a runtime registry?** Yes. B2 needs no registry, no RAM beyond the
   application's own objects and no registration code, and it produces the same final image as hand-written
   code on GCC, Clang and Cortex-M33, within one TU and across TUs with or without LTO.

### Recommended hot-loop coding pattern (draft, for review)
1. Write receivers as plain classes with non-virtual `receive(const T&)`, and `filter()` only where it filters.
2. Bind instances at the composition point:
   - **static topology (`StaticWiring`)** for objects with static storage duration, the common embedded case,
     which is proven zero-cost;
   - **`wire(...)`** when object lifetimes are dynamic, which costs one stored address per binding.
3. Make hot-path publishers generic over their output (`template<class Out>`). Use `Sink<T>` only where a
   non-template publisher is required (for example across a library or ABI boundary), accepting one
   indirect call.
4. Put genuinely dynamic subscribers behind a runtime registry at a dynamic boundary. A bridge element that
   joins the two is next, and it will be measured.

### Compromises and open items (pattern B)
- **Ergonomics:** hot-path publishers become templates, or accept one indirect call through `Sink<T>`. This
  is the "specific Sub0Pub-compliant manner" of coding the issue anticipated.
- **B2 requires static storage duration** (addresses are template arguments). B1 covers dynamic
  lifetimes at the measured binding cost.
- **Not yet in pattern B:** cancellation (a receiver-controlled early stop), a static-to-dynamic bridge,
  B3 rows for the new cases, and MSVC evidence (`dumpbin`).
- **B1's reference is static-address hand-written code,** so its delta mixes the binding cost with any
  abstraction cost. An equal-work hand-written version with runtime addresses would separate them.
- **The static path metric follows direct calls only.** Work behind an indirect call appears as an
  indirect-call count, not as instructions (for example pattern A's cross-file path on Cortex-M33).

### Implications for the v2 API (#8)
- The fastest structure is **not a broker policy**; it is where the application composes itself. v2
  therefore needs composition-point wiring as a first-class public facility next to the runtime broker,
  not only per-message broker options.
- Per-message configuration (#8) remains the right place for *policy*: capacity, locking, reentrancy and
  scoping for runtime registries. The static path needs no per-message configuration at all.
- #8's broker specialisation must be able to hand a message type or domain to the static structure. The
  bridge between static wiring and runtime registries is the next measurement.

## Plan (issue #9)
- **Phase 1: sandbox (first round done, above).** Next:
  - the static-to-dynamic bridge;
  - cancellation in the static path;
  - publisher ergonomics alternatives (for example CRTP publishers, `auto&` sinks, C++20 concepts);
  - MSVC evidence;
  - the equal-work runtime-address reference for B1.
- **Phase 2:** broker specialisation (#8) selects the chosen static structure per message type or domain,
  with runtime subscription kept at the dynamic boundary.
- **Phase 3:** decision record here and in BROKER_CUSTOMISATION.md; compromises go into its known-issues list.

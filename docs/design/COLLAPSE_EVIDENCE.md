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
   - `handwritten_<kind>.cpp` are further equal-work references for patterns that do more than direct calls,
     so each pattern is judged against what a careful engineer writes *for the same job*:
     `handwritten_runtime` (receiver addresses stored at setup, called through), `handwritten_erased`
     (a C-style context pointer plus function pointer), `handwritten_registry` (a hand-written dynamic
     registry). A variant selects one with a leading `// SUB0X_REFERENCE: handwritten_<kind>` line; each extra
     reference is itself reported against `handwritten`, which prices the choice (runtime binding, type
     erasure, a dynamic registry) independently of any library;
   - every other `.cpp` implements the *same behaviour* through a Sub0Pub coding pattern (a *variant*),
     in its best form (for example the #8 registry in the leanest configuration the scenario allows, beside
     its default).

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
| **B1 runtime-bound wiring** | `auto bus = sub0x::wire(controllerA, controllerB, logger);`; publishers are templated on their output and hold it by value (`template<class Out> struct Sensor { Out out; ... }`: a `Wiring` is a tuple of receiver references) | never (addresses are runtime values, types are static) |
| **B3 type-erased publisher port** | a non-template publisher holds `sub0x::Sink<Sample>` constructed from a wiring | at the publisher: exactly one indirect call, everything behind it typed |
| Transport endpoint | `sub0x::Forward<Radio>` (held by value in a `Wiring`) / `sub0x::StaticForward<&radio>` bound like a receiver; ingress `bus.publishFrom(endpoint, msg)` or `bus.publishFrom<Endpoint>(msg)` skips that endpoint | never |

**Against hand-written equal-work code** (deltas; "=" means identical on every criterion). Each pattern is
compared with the reference that does the same job (see "The loop"); the price of the job itself, paid by
hand-written code too, is in the last column.

| Case | B2 static (vs `handwritten`) | B1 wire (vs `handwritten_runtime`) | B3 sink (vs `handwritten_erased`) | Pattern A (today) vs `handwritten` | Price of the job, hand-written |
|---|---|---|---|---|---|
| zero / one / multi receivers, filters | **= on all builds** | **= on all builds** | **=** (Clang inlines the erased call: publish -1, larger static path) | publish +0 to +117; leanest macros (`SUB0PUB_REENTRANT_SAFE=0`, `SUB0PUB_ASSERT=0`) +0 to +100 | runtime binding: publish -1 to +8, RAM +0 to +32 B; type erasure: publish +0 to +22, RAM +12 to +40 B |
| two domains | **= on all builds** | **= on all builds** | not measured | not expressible (#8 registry: +143 to +169; lean +63 to +71, against static hand-written code) | runtime binding as above |
| transport endpoint | **= on all builds** | **= on all builds** | not measured | not expressible (#8 registry with a route: +175 to +209; lean +112 to +116, against static hand-written code) | runtime binding as above |
| cross-file, no LTO | **= on all builds** | **= on all builds** | not measured | +69 to +95; leanest macros +53 to +78 | runtime binding: none |
| cross-file, LTO | **= on all builds** | **= on all builds** | not measured | +96 to +115, leanest +80 to +98: LTO does not devirtualise the runtime registry | runtime binding: +0 to +3 |

Dynamic subscriptions, against a hand-written dynamic registry with the same features (8 slots, add/remove,
no filter, no snapshot, no cancel): publish +13.0 (GCC) and +6.2 (Clang) for the #8 registry in its lean
configuration (`config<Direct, NoContext, NoFilter>`), and +46.5 / +55.5 in its default configuration, whose
snapshot, cancel context and filter the hand-written registry does not have. Today's API: +19.3 / +36.8 at
its leanest macros, +32.5 / +46.3 by default. The runtime path stays supported, at an explicit, measured cost.

**Fairness review (2026-09).** The first Phase 1 round compared B1 and B3 with static-address hand-written code,
so their deltas mixed the price of runtime binding or type erasure (paid by hand-written code too) with the
pattern's own overhead, and several variants were not in their best form. Corrections, each re-measured
([../perf/collapse/phase1-fair-references-2026-09.md](../perf/collapse/phase1-fair-references-2026-09.md)):
- B1 publishers held `const Out&` to a separate wiring object (two hops); they now hold the wiring by value.
- `Wiring` read every binding up front through `std::apply`, keeping them live across the calls (+6 instr
  cross-file); it now reads each binding just before its delivery, as hand-written code does.
- A `Forward` transport adapter was held by reference (an extra hop); adapters now declare themselves
  by-value, and split horizon identifies an endpoint through the object it refers to. When the origin's type is
  bound once, the skip is decided at compile time (`publishFrom<Endpoint>(msg)`, or the object form with a
  debug assertion that the origin is bound; known issue K15).
- The #8 registry and today's API were measured only in their default configurations; their leanest valid
  configurations are now measured beside them.

With these, **B1 and B3 cost nothing over hand-written code doing the same job**, on every build, case and form.

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
   - **`wire(...)`** when object lifetimes are dynamic: exactly what hand-written runtime binding costs (one
     stored address per binding), nothing more. Publishers hold the returned wiring by value.
3. Publishers, in order of cost:
   - name the `StaticWiring` alias directly (`=` on every build);
   - use the CRTP mixin `sub0x::Publisher<Derived, Out>` when the topology is not known where the publisher
     is written (`=` against hand-written runtime binding, like a hand-spelled `template<class Out>`);
   - use `Sink<T>` across a library or ABI boundary (`=` against a hand-written context + function pointer:
     the one indirect call is the price of erasure itself).
4. A receiver that stops the rest of a publication returns `bool` (`false` stops), with `publishCancelable`
   (`=` on every build).
5. Put genuinely dynamic subscribers behind a `DynamicPort<T, N>` bound into the static wiring (`=` against a
   hand-written registry with the same features, populated or empty), or behind a `BrokerPort<T>` to the #8
   registry when they need policy (publish `=` on GCC and Clang in its lean configuration; setup, teardown,
   image and RAM cost more).

The face-offs behind points 3-5 are recorded in [spikes/README.md](spikes/README.md).

### Compromises and open items (pattern B)
- **Ergonomics:** hot-path publishers become templates, or accept one indirect call through `Sink<T>`. This
  is the "specific Sub0Pub-compliant manner" of coding the issue anticipated.
- **B2 requires static storage duration** (addresses are template arguments). B1 covers dynamic
  lifetimes at the measured binding cost.
- **Not yet in pattern B:** B3 rows for the new cases, and MSVC evidence (`dumpbin`). Cancellation and the
  static-to-dynamic bridge are done ([spikes/README.md](spikes/README.md)).
- **Choosing runtime binding or type erasure has a price** (runtime binding: publish up to +8, RAM up to
  +32 B; erasure: up to +22, +40 B, against static code), but it is the price of the choice, identical in
  hand-written code. The earlier K11 and K12 entries measured that price, not a Sub0Pub overhead, and are
  withdrawn.
- **Capability routing drops silently on a signature mismatch.** A receiver whose `receive` does not match
  the message (wrong parameter type, or non-const where the wiring is const) is simply not delivered to.
  The fairness review hit exactly this with a by-value adapter. Known issue K14.
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
  - ~~the static-to-dynamic bridge; cancellation in the static path; publisher ergonomics alternatives~~
    (done, [spikes/README.md](spikes/README.md));
  - MSVC evidence;
  - ~~the equal-work runtime-address reference for B1~~ (done, with `handwritten_erased` and
    `handwritten_registry`: see "Fairness review").
- **Phase 2:** broker specialisation (#8) selects the chosen static structure per message type or domain,
  with runtime subscription kept at the dynamic boundary.
- **Phase 3:** decision record here and in BROKER_CUSTOMISATION.md; compromises go into its known-issues list.

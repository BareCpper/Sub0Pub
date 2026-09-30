# C++23 and integration groundwork

This groundwork moves the v2 core to C++23. It does not add a Sub0Pipeline dependency or implement an adapter.
The existing public messaging model remains synchronous, with static wiring and opt-in runtime broker policies.

## Language and build contract

- `Sub0Pub::Sub0Pub` exports `cxx_std_23` for source-tree and installed consumers.
- Direct header consumers must enable C++23 mode. The common configuration header rejects C++17 and C++20;
  draft C++23 language-date macros are accepted, including MSVC's `_MSVC_LANG` reporting.
- Capability detection uses requires-expressions. Explicit boolean filter conversions, exact-bool cancellation,
  ignored nonmatching receivers and Sink copy semantics are preserved and covered through public APIs.
- Standalone footprint, comparison and collapse runners use C++23 (MSVC uses `/std:c++latest`). Both sides of
  an equal-work comparison use the same mode, including historical v1 source when rebuilding a comparison.
- The package-consumer test starts with a C++17 application default and verifies the imported target raises it
  to C++23. It also compiles a C++23 standard-library operation, so exporting the feature string alone is insufficient.

Language mode is not a guarantee of complete compiler support for every C++23 feature. CI exercises the project's
actual usage on GCC, Clang, AppleClang and MSVC, plus Cortex-M33 evidence builds. Deducing-this, reflection and
other features are not prerequisites merely because the standard baseline changed.

## Modernization review

| Area | Groundwork decision | Reason / follow-up |
|---|---|---|
| Static wiring detection | Replace expression-SFINAE with requires-expressions | Direct expression of capability; no runtime machinery |
| Sink copy exclusion | Constrain the binding constructor with `requires` | Keeps ordinary copy construction from binding to another Sink |
| Broker configuration detection | Retain existing implementation | Contains documented MSVC ADL workarounds; removal needs focused cross-compiler evidence |
| Public detection utilities | Retain names and behavior | C++23 adoption does not justify breaking consumer utilities |
| Publisher CRTP | Retain it | Explicit-object parameters need toolchain coverage and equal-work evidence before replacement |
| Enums/results | Retain small result enums | `std::expected` is useful at a future admission boundary, not a reason to change synchronous publication |
| Payload interfaces | Retain `const T&` | Queue ownership belongs in an explicit adapter, not in ordinary fan-out |
| Static array bindings | Remove obsolete universal compile-fail assertion | GCC 13 accepts array-element NTTPs in C++23; use named objects or `wire` for portable bindings |
| Synchronization and storage | No policy changes | Atomics, mutexes and bounded tables have lifetime and cost contracts independent of language spelling |

The move does not claim an across-the-board performance improvement. Historical reports under `docs/perf/`
retain their original flags and results. Current runners and CI produce fresh C++23 evidence; existing budgets
remain gates rather than being silently widened. Recorded C++17 numbers are not new C++23 measurements.

## Sub0Pipeline integration boundary

The reviewed baseline is Sub0Pipeline main at `f6f54c623908649e8daac3613545062cf08b3822`.
Its [public interfaces](https://github.com/CraigHutchinson/Sub0Pipeline/blob/f6f54c623908649e8daac3613545062cf08b3822/include/sub0pipeline/sub0pipeline.hpp)
and [implementation](https://github.com/CraigHutchinson/Sub0Pipeline/blob/f6f54c623908649e8daac3613545062cf08b3822/src/sub0pipeline.cpp)
support a first-class optional integration through public APIs. Both libraries can now share a C++23 baseline;
that removes a packaging mismatch but does not settle adapter ownership.

Prefer typed lifecycle output first: an `IObserver` implementation publishes to a supplied wiring or Sink.
For message-to-workflow execution, use a distinct bounded admission/ownership contract. Ordinary publication
returns no admission result and does not transfer payload ownership. Important constraints at the reviewed revision:

- `trigger()` dispatches one on-demand job, not its successor DAG; duplicate active submissions can return `kBusy`.
- On-demand execution emits start/finish events but not the DAG failure-detail hook.
- Observer callbacks may overlap, and names are borrowed views rather than stable per-invocation IDs.
- Terminal status can precede safe reclamation of timed-out work. Completion and orphan joining must remain explicit.
- Sub0Pub cancellation stops a publication; Pipeline cancellation requests stopping work. They are different contracts.
- `IExecutor::dispatch` must eventually invoke completion. A bus publication with optional/multiple recipients cannot
  serve as a general executor without a separate admission and exactly-once completion protocol.

No queue, scheduler hook, new public integration namespace or unused extension abstraction is added in this PR.

## Where the adapter should live

| Home | Advantages | Costs / appropriate scope |
|---|---|---|
| Sub0Pub extension | Discoverable to messaging consumers; nearby wiring expertise | Scheduler lifecycle maintenance in the messaging project; isolate optional dependency |
| Sub0Pipeline extension | Observer semantics and tests nearby; least new release process | Adapter release follows Pipeline; cross-link documentation from Sub0Pub |
| Dedicated integration package | Neutral ownership and independent bridge releases | Third package and compatibility matrix; useful when the bridge is substantial |
| Umbrella project | Common entry point, tested version combinations, component adapters and examples | Broader support responsibility; justified by a reusable toolkit, not a few forwarding functions |

For a narrow lifecycle adapter, Sub0Pipeline remains the preferred initial home. If the intended scope is a reusable
Sub0 composition toolkit, an umbrella-owned component is a credible alternative. Its dependencies point toward
both standalone libraries; neither core depends on the umbrella. An umbrella can also distribute an existing
Pipeline-owned adapter without moving or duplicating it.

Keep one canonical bridge implementation and contract suite. Use optional component targets, pinned compatibility
tests, installed-package checks and explicit owner/platform choices. Do not impose a global bus, executor,
allocator or automatic dependency downloads. Choosing C++23 does not authorize an umbrella or adapter implementation.

## Future acceptance gates

Before implementing the integration, settle owner/package and its first use case. Validate payload lifetime,
concurrent observation, overload reporting, wakeup races and teardown with controlled executors. Compare against
equal-work handwritten glue, including the same ownership and queue policy. Bounded ingress must not be described
as heap-free or ISR-safe execution: those guarantees need separate scheduler/platform work.

## Groundwork validation (30 September 2026)

Compile-time A/B evidence is captured separately in [COMPILE_TIME.md](COMPILE_TIME.md): full C++17-to-C++23
migration and same-language source comparison, with repeated multi-TU wiring, broker and umbrella workloads.
The initial groundwork CI passed GCC, Clang, AppleClang, MSVC, sanitizers and the full collapse-evidence job.

Local GCC 13.3.0 Release validation: all 252 CTest entries passed, including enabled examples, header isolation,
compile-fail contracts, rejection of C++17/C++20, and the installed consumer. The evidence parser/gate unit suite
passed all 11 tests.

A focused `gcc-O2` / `filters` collapse comparison used parent `ab3622277018b7d18cc507afad64af1d1bfbe6a6`
in C++17 mode and this groundwork in C++23 mode, on the same local toolchain. All 18 records (nine variants,
observable/removable forms) had identical checksums, text/data/bss/init-array sizes, publish-path instruction/call
counts and reported dependencies. This is static image and behavior evidence, not measured dynamic instruction counts.

The historical budget invocation does **not** pass locally: Valgrind is unavailable, so publish/setup/teardown
instruction measurements are missing; runtime broker variants also exceed the recorded text/RAM deltas. Those
section sizes are identical in the parent and changed builds on this host. Do not label this a passed full
performance gate or increase budgets from this run. CI supplies Valgrind and the broader compiler/embedded matrix;
its results must be reviewed before merging.

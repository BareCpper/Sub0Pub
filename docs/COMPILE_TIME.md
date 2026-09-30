# Compile-time performance

Build time is a separate performance dimension from publish latency, code size and RAM. Sub0Pub is header-only:
include parsing and template instantiation repeat across consumer translation units. Changes to public headers,
constraints, configuration machinery or the language baseline should include compile-time A/B evidence when they
can materially affect those costs.

## Reproduce the C++23 groundwork comparison

Run from a checkout with both commits available, using Python 3 and a direct GCC/Clang compiler executable:

```sh
# A: prior v2/C++17. B: C++23 groundwork. This measures the complete migration effect.
python3 tests/compile_time/compare.py \
  --baseline ab3622277018b7d18cc507afad64af1d1bfbe6a6 \
  --candidate 1e8f1dcef8ed29fa534769625bec6965c9bb42fb \
  --translation-units 8 \
  --json migration.json --markdown migration.md

# Hold language mode constant to separate the source changes from the language-mode change.
python3 tests/compile_time/compare.py \
  --baseline ab3622277018b7d18cc507afad64af1d1bfbe6a6 \
  --candidate 1e8f1dcef8ed29fa534769625bec6965c9bb42fb \
  --baseline-standard c++23 --translation-units 8 \
  --json source.json --markdown source.md
```

For later C++23 changes, use `--baseline <base-ref> --candidate <head-ref> --baseline-standard c++23`.
Refs are resolved to full commit IDs before measurement. Only archived headers from those commits are used;
uncommitted library edits are **not** measured. The harness and fixture come from the invoking checkout and their
SHA-256 hashes are recorded. Both lanes always use the exact same fixture and compiler.

Recorded groundwork results: [migration](perf/compile-time/cxx23-migration.md) / [raw samples](perf/compile-time/cxx23-migration.json),
and [same-language source comparison](perf/compile-time/cxx23-source.md) / [raw samples](perf/compile-time/cxx23-source.json).

### Observed results for this changeset

| Workload | Migration: C++17 baseline → C++23 candidate | Source comparison: both C++23 |
|---|---:|---:|
| wiring | +58.68% | -14.64% |
| broker | +61.83% | +0.93% |
| umbrella | +82.76% | +16.00% |

Positive means slower. The migration capture used an AMD EPYC host; the later same-language capture used an
Intel Xeon host after the execution environment changed. Each individual A/B comparison uses one host/compiler,
but **do not subtract these columns or compare their absolute seconds** to assign a precise language-only cost.

The migration's observed ranges do not overlap: it is a meaningful compile-cost warning for this workload,
not a claim that every application will build 59–83% slower. The same-language capture has substantial noise
and overlapping ranges in every profile. Its mixed results do not establish a general source-level improvement
or regression. Review further CI captures or repeat on a quiet fixed host before making that claim. Keep the
groundwork PR under review with this cost visible; the runtime performance gates do not cover compilation cost.

## Workloads

| Profile | Includes | Instantiated work |
|---|---|---|
| wiring | `sub0pub/wiring.hpp` | Multiple message types and receivers, fan-out, Sink construction/copy/publication |
| broker | `sub0pub/broker.hpp` | Per-message subscriptions, virtual receive, publisher and registration/teardown |
| umbrella | `sub0pub/sub0pub.hpp` | Exactly the wiring work, through the full include surface |
| layout | `sub0pub/utility/layout.hpp` | Distinct 32-member aggregates and recursive array fingerprints |

The umbrella/wiring pair helps expose the cost of the full include surface, but the principal comparison is
**A versus B within each profile**. Broker and wiring do different work and their absolute timings are not a
dispatch-performance ranking. Each TU uses distinct message types, representing independent consumer components.
The layout profile varies with `--types`; `--receivers` does not affect it.
This is a synthetic consumer compile workload; it does not time this repository's test suite.

The default is 16 translation units per batch, eight message types per TU, four receivers, five recorded paired
samples and one warmup batch per lane/profile. The committed capture and CI use eight TUs to limit run time.
`--translation-units`, `--types` and `--receivers` vary repeated parsing and template breadth independently;
receiver counts are limited to the broker's default capacity of eight. For a larger local study, use 16/64 TUs
and retain each report separately. Do not simply extrapolate these serial times into parallel application builds.

## What the timer includes

- Every TU is compiled serially with `-O2 -DNDEBUG -pthread`, the selected standard, and identical workload defines.
- Compilation starts in a fresh compiler process and produces a fresh object; timing includes process startup,
  preprocessing, parsing, template instantiation, optimization and object emission.
- Snapshot extraction, source generation, object cleanup, warmups, configure and link are outside the timer.
- Filesystem caches are warm after warmups. This is a **clean object compilation**, not a cold machine/disk cache test.
- No PCH, modules, unity builds, ccache or sccache. Invoke the real compiler; arbitrary custom wrappers are unsupported.
- No parallel compiler scheduling: this isolates aggregate compiler work from build-system parallelism and memory contention.

The tool creates temporary snapshots and never edits either source revision. Failed compiler processes or missing
object outputs abort the run rather than becoming a fast timing. The quick harness contract tests are in CTest;
the expensive timed benchmark is opt-in and has its own CI workflow.

## Interpretation and review

Samples alternate A/B then B/A, including warmups, to reduce order/thermal drift. JSON retains every measured pair
and its order, compiler/version, CPU/platform/affinity, include environment, flags, exact source revisions and
fixture/harness hashes. Reports show median, min/max, median absolute deviation (MAD), and percentage change
of B's median relative to A. Positive change means slower. No outliers are silently removed.

Ranges and MAD are descriptive, not confidence intervals. Small deltas within observed variation are inconclusive.
Use a quiet machine, rerun suspicious changes, and compare within the same run. Shared CI hosts add noise; do not
treat a small timing delta as a hard failure or compare absolute seconds across hosts/compiler versions.

The `Compile time A/B` workflow captures the same-language comparison on v2 PRs and also the migration comparison
when the base declares C++17. It uploads raw JSON and Markdown and adds a job summary. Compilation/harness failures
fail the job, but there is deliberately no arbitrary wall-clock percentage gate. Meaningful regressions should
be investigated and justified alongside the existing runtime/footprint gates.

These results do not measure MSVC, parallel build latency, incremental/no-op rebuilds, linker cost, peak compiler
memory or application-specific include graphs. Those are distinct studies if a consumer's build profile warrants them.

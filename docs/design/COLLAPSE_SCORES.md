# #10 collapse axes: score matrix and coverage

Every design axis that PR #10 (issue #9, "collapse gate") compares, scored option by option on measured cost and on
the guarantees each option gives, in the structure of the #8 policy-axes matrix ([AXIS_SCORES.md](AXIS_SCORES.md)).
The second half asks whether the collapse cases are exhaustive enough to show each option's merits and limitations,
and records the gaps found and closed, and the defects the review found in the sandbox and in the evidence tool.

## Method

- **Evidence.** One full run of `python3 tests/collapse/collapse_evidence.py --json ...` over every case, variant,
  form and named build (gcc-O2, clang-O2, cm33-gcc-Os, and the LTO builds for multi-TU cases), stored as
  [../perf/collapse/phase1-scores-2026-09.md](../perf/collapse/phase1-scores-2026-09.md) and
  [.json](../perf/collapse/phase1-scores-2026-09.json). The run exits 0 under the strict gating added by this review
  (no behaviour mismatch, no build, run or measurement failure, no missing reference); the only omissions are the
  explicit feature skips it lists (deducing this on GCC 13 and `arm-none-eabi-g++` 13, `std::expected` on
  clang 18 + libstdc++ 13).
- **Axes.** An axis is a design dimension #10 compares; its options are case variants. Each axis is scored over the
  cases in which its options coexist, including the axis's hand-written references (so "cheapest" can be hand-written
  code). The axes and their cases are declared in `tests/collapse/collapse_scores.py` (`AXES`).
- **Tables are generated**, not hand-computed: `python3 tests/collapse/collapse_scores.py
  docs/perf/collapse/phase1-scores-2026-09.json` prints every table in "Score tables" below, and a raw-value table per
  axis (collapsed) so each score can be traced to a measurement.
- **Guarantees** cite the case, test or compile-fail check that demonstrates them; tests added by this review are in
  `tests/collapse/unit/` (`Sub0Pub_CollapseSandboxTests` and `Sub0Pub_Collapse_cf_*`).

**Score (0 to 3), per axis and metric, against the cheapest option of the same axis in the same case:** the geometric
mean over the axis's cases of *option / cheapest*: **3** ≤ ×1.10, **2** ≤ ×1.35, **1** ≤ ×2.0, **0** above (the
AXIS_SCORES.md rubric). Metrics, observable-work form:

| Metric | Source |
|---|---|
| gcc / clang publish | callgrind instructions per publication, gcc-O2 / clang-O2 |
| setup+teardown | callgrind instructions of one construction plus one destruction, gcc-O2 |
| cm33 path | static instructions on the publish path of the final Cortex-M33 ELF, compared only among options making the same number of indirect calls (work behind an indirect call is invisible to the metric); the cell shows the most indirect calls |
| cm33 text / RAM | application bytes: the image minus the `zero_receivers/handwritten` image (driver and C runtime), so the C runtime does not dilute the ratio |
| cm33 deps | link dependencies added over the case's `handwritten`: **3** none, **1** one, **0** two or more (worst case) |
| vs its equal-work reference | the `collapse_evidence.py` criteria against the reference the variant declares (`// SUB0X_REFERENCE:`), over every build and both forms: **=** means every criterion met everywhere; otherwise passes/total and the most frequent failures |

Noise floors: a difference within 2 instructions, 16 B of text or 8 B of RAM counts as equal, and byte denominators are
floored at 64 B (text) and 16 B (RAM). Scores rank options *within* an axis. **The cost score and the reference column
answer different questions:** the score prices the choice (runtime binding costs more than static addresses, whoever
writes it), the reference column says whether the pattern adds anything over hand-written code doing the same job.
Cost verdicts are report-only in the tool (pattern A is expected to fail them); only behaviour and measurement
failures fail a run.

## Axes

Confirmed from the code (cases, sandbox headers, spike documents); the brief's list is kept, with two refinements:
filters are not a separate axis in #10 (every binding form has the same compile-time `filter()` capability; the
`filters` case is one of the binding-form cases), and "publishFrom object vs type" is part of the transport axis.

| Axis | Options (variants) | Cases |
|---|---|---|
| Binding form | B2 `StaticWiring`, B1 `wire(...)`, B3 `Sink<T>`, A today's API (default, leanest macros), #8 registry (default, leanest valid configuration), and the hand-written static / runtime-address / erased references | zero, one, multi receivers, filters, many_receivers, multi_types, large_payload, nested_publish, two_domains, transport_endpoint, cross_file, dynamic_subscriptions |
| Publisher spelling | alias of the `StaticWiring`, hand `template<class Out>`, CRTP mixin, CTAD factory, call-site argument, `Sink<T>`, deducing-this mixin and call site (C++23) | publisher_ergonomics |
| Static-path cancellation | `bool receive()` (B2, and B1 new), `std::expected` (C++23), `Delivery&` token, `cancel()` flag (static, thread-local), filter on a shared flag | cancellation, cancellation_filtered |
| Static/dynamic bridge | `DynamicPort<T, N>` (and C++23 bind result), `BrokerPort<T>`, inverted `StaticAdapter` | static_dynamic_bridge, _empty, _churn |
| Transport endpoints, split horizon | `StaticForward` and `Forward`; origin named by the binding, by type, or by the transport object; B3 through `Sink`; #8 `Route` | transport_endpoint, transport_two_links |
| Domains and sessions | one wiring per domain (B2, B1, B3), one publisher holding both wirings (B2, B1), #8 `Domain` | two_domains |
| Cross-TU and LTO | B2, B1, B3, A with and without LTO | cross_file (gcc, clang, Cortex-M33, each with and without `-flto`) |
| C++ standard of the spelling | C++17 spelling vs its C++20 (concepts) or C++23 (`std::expected`, deducing this) counterpart | filters, multi_receivers, cancellation, static_dynamic_bridge, publisher_ergonomics |

## Score tables

<!-- generated by tests/collapse/collapse_scores.py from phase1-scores-2026-09.json -->

### Binding form

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static (`handwritten`) | 12 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **2** ×1.14 (1 indirect) | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written, runtime addresses (`handwritten_runtime`) | 10 | **2** ×1.10 | **3** ×1.01 | **2** ×1.20 | **2** ×1.16 | **2** ×1.12 | **2** ×1.22 | **3** | 11/60 (vs `handwritten`; fails: no extra RAM 48, setup instr 29, publish path 9) |
| hand-written, context + function pointer (`handwritten_erased`) | 10 | **1** ×1.74 | **1** ×1.42 | **2** ×1.31 | **3** ×1.02 (2 indirect) | **1** ×1.50 | **1** ×1.57 | **3** | 0/60 (vs `handwritten`; fails: no extra RAM 60, no extra indirect calls 55, setup instr 40) |
| B2 `StaticWiring` | 11 | **3** ×1.00 | **3** ×1.01 | **3** ×1.00 | **2** ×1.15 | **3** ×1.00 | **3** ×1.00 | **3** | 64/66 (vs `handwritten`; fails: publish path 2, no Sub0Pub retained 1, publish instr 1) |
| B1 `wire(...)` | 11 | **3** ×1.09 | **3** ×1.01 | **2** ×1.18 | **2** ×1.15 | **2** ×1.11 | **2** ×1.20 | **3** | 65/66 (vs `handwritten`, `handwritten_runtime`; fails: publish path 1, no Sub0Pub retained 1) |
| B3 `Sink<T>` | 10 | **1** ×1.74 | **1** ×1.41 | **2** ×1.31 | **3** ×1.02 (2 indirect) | **1** ×1.50 | **1** ×1.57 | **3** | 51/60 (vs `handwritten_erased`; fails: publish path 9) |
| A today's API, default macros | 10 | **0** ×3.03 | **0** ×3.56 | **0** ×4.16 | **0** ×6.21 (2 indirect) | **0** ×15.10 | **0** ×18.97 | **0** TLS, operator delete | 0/60 (vs `handwritten`; fails: no extra RAM 60, no Sub0Pub retained 60, no extra dependencies 60) |
| A today's API, leanest macros | 10 | **0** ×2.25 | **0** ×3.25 | **0** ×4.16 | **0** ×2.01 (2 indirect) | **0** ×12.96 | **0** ×16.54 | **0** operator delete, TLS | 0/60 (vs `handwritten`; fails: no extra RAM 60, no extra dependencies 60, no Sub0Pub retained 58) |
| #8 registry, default configuration | 11 | **0** ×4.03 | **0** ×5.15 | **0** ×5.34 | **0** ×4.38 (2 indirect) | **0** ×16.32 | **0** ×21.26 | **0** TLS, operator delete | 0/66 (vs `handwritten`; fails: no extra RAM 66, no Sub0Pub retained 66, no extra dependencies 64) |
| #8 registry, leanest valid configuration | 11 | **0** ×2.33 | **0** ×2.40 | **0** ×4.74 | **2** ×1.34 (2 indirect) | **0** ×11.32 | **0** ×7.33 | **1** operator delete | 6/66 (vs `handwritten`; fails: no extra RAM 60, no Sub0Pub retained 60, no extra dependencies 60) |

<details><summary>Binding form: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `zero_receivers` | `one_receiver` | `multi_receivers` | `filters` | `many_receivers` | `multi_types` | `large_payload` | `nested_publish` | `two_domains` | `transport_endpoint` | `cross_file` | `dynamic_subscriptions` |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| hand-written, static (`handwritten`) | 8.0 / 8.0 / 34 / 7 / 0 / 0 | 15.0 / 18.0 / 35 / 15 / 32 / 4 | 30.0 / 33.0 / 37 / 30 / 92 / 12 | 19.0 / 22.0 / 34 / 19 / 32 / 0 | 202.0 / 256.0 / 66 / 143 / 728 / 128 | 32.0 / 34.0 / 35 / 30 / 72 / 4 | 67.0 / 26.0 / 35 / 35 / 76 / 4 | 56.0 / 61.5 / 34 / 40 / 84 / 0 | 31.0 / 34.0 / 37 / 30 / 92 / 12 | 27.0 / 29.0 / 34 / 25 / 52 / 0 | 61.0 / 55.0 / 37 / 43 / 124 / 12 | 60.8 / 57.0 / 62 / 162 / 540 / 40 |
| hand-written, runtime addresses (`handwritten_runtime`) | - | 16.0 / 17.0 / 37 / 17 / 44 / 8 | 38.0 / 33.0 / 43 / 32 / 104 / 24 | 19.0 / 22.0 / 38 / 19 / 52 / 12 | 296.0 / 265.0 / 130 / 114 / 744 / 256 | 36.0 / 34.0 / 41 / 32 / 100 / 24 | 70.0 / 26.0 / 39 / 37 / 96 / 16 | 56.0 / 65.5 / 42 / 40 / 116 / 20 | 38.0 / 34.0 / 43 / 37 / 124 / 24 | 27.0 / 29.0 / 38 / 25 / 72 / 12 | 61.0 / 55.0 / 43 / 42 / 128 / 24 | - |
| hand-written, context + function pointer (`handwritten_erased`) | 22.0 / 8.0 / 38 / 15 / 44 / 12 | 31.0 / 26.0 / 41 / 15 / 80 / 16 | 52.0 / 45.0 / 47 / 15 / 144 / 32 | 35.5 / 31.0 / 42 / 15 / 96 / 20 | 282.0 / 274.0 / 134 / 15 / 844 / 264 | 67.0 / 59.0 / 48 / 23 / 172 / 40 | 77.0 / 53.0 / 43 / 22 / 128 / 24 | - | 65.0 / 58.0 / 51 / 27 / 204 / 40 | 45.0 / 43.0 / 42 / 25 / 128 / 20 | 74.0 / 67.0 / 47 / 17 / 168 / 32 | - |
| B2 `StaticWiring` | 8.0 / 8.0 / 34 / 7 / 0 / 0 | 15.0 / 18.0 / 35 / 15 / 32 / 4 | 30.0 / 33.0 / 37 / 30 / 92 / 12 | 19.0 / 22.0 / 34 / 19 / 32 / 0 | 202.0 / 256.0 / 66 / 143 / 728 / 128 | 32.0 / 34.0 / 35 / 30 / 72 / 4 | 67.0 / 26.0 / 35 / 35 / 76 / 4 | 54.5 / 65.5 / 34 / 40 / 84 / 0 | 31.0 / 34.0 / 37 / 30 / 92 / 12 | 27.0 / 29.0 / 34 / 25 / 52 / 0 | 61.0 / 55.0 / 37 / 43 / 124 / 12 | - |
| B1 `wire(...)` | 8.0 / 8.0 / 34 / 7 / 0 / 0 | 16.0 / 17.0 / 37 / 17 / 44 / 8 | 38.0 / 33.0 / 43 / 32 / 104 / 24 | 19.0 / 22.0 / 38 / 19 / 52 / 12 | 296.0 / 265.0 / 130 / 114 / 744 / 256 | 36.0 / 34.0 / 41 / 32 / 100 / 24 | 70.0 / 26.0 / 39 / 37 / 96 / 16 | 54.5 / 65.5 / 42 / 41 / 120 / 20 | 38.0 / 34.0 / 43 / 37 / 124 / 24 | 27.0 / 29.0 / 38 / 25 / 72 / 12 | 61.0 / 55.0 / 43 / 42 / 128 / 24 | - |
| B3 `Sink<T>` | 22.0 / 8.0 / 38 / 15 / 44 / 12 | 31.0 / 25.0 / 41 / 15 / 80 / 16 | 52.0 / 44.0 / 47 / 15 / 144 / 32 | 35.5 / 30.0 / 42 / 15 / 96 / 20 | 282.0 / 273.0 / 134 / 15 / 844 / 264 | 67.0 / 59.0 / 48 / 23 / 172 / 40 | 76.0 / 52.0 / 43 / 22 / 128 / 24 | - | 65.0 / 58.0 / 51 / 27 / 204 / 40 | 45.0 / 43.0 / 42 / 25 / 128 / 20 | 74.0 / 66.0 / 47 / 17 / 168 / 32 | - |
| A today's API, default macros | 18.0 / 8.0 / 37 / 30 / 720 / 408 | 47.0 / 78.0 / 83 / 146 / 1572 / 152 | 129.0 / 150.0 / 184 / 162 / 1844 / 440 | 122.5 / 113.0 / 133 / 168 / 1836 / 424 | 434.0 / 1007.0 / 2077 / 146 / 2392 / 620 | 187.0 / 221.0 / 242 / 209 / 2416 / 496 | 141.0 / 138.0 / 134 / 169 / 1840 / 424 | 209.0 / 283.5 / 194 / 170 / 2292 / 480 | - | - | 130.0 / 150.0 / 184 / 178 / 1928 / 440 | 93.3 / 103.3 / 82 / 203 / 1844 / 416 |
| A today's API, leanest macros | 8.0 / 8.0 / 37 / 7 / 660 / 104 | 26.0 / 71.0 / 83 / 31 / 1268 / 152 | 99.0 / 133.0 / 184 / 44 / 1528 / 440 | 94.5 / 94.5 / 133 / 50 / 1520 / 424 | 336.0 / 1001.0 / 2077 / 31 / 2088 / 620 | 134.0 / 185.0 / 242 / 76 / 2064 / 496 | 117.0 / 120.0 / 134 / 51 / 1524 / 424 | 209.0 / 283.5 / 194 / 170 / 2292 / 480 | - | - | 114.0 / 133.0 / 184 / 58 / 1608 / 440 | 80.0 / 93.8 / 82 / 83 / 1532 / 416 |
| #8 registry, default configuration | 27.0 / 41.0 / 34 / 152 / 384 / 300 | 53.0 / 82.0 / 104 / 148 / 1640 / 404 | 137.0 / 163.0 / 241 / 162 / 1868 / 428 | 131.5 / 121.0 / 168 / 171 / 1864 / 412 | 528.0 / 1137.0 / 2791 / 148 / 2456 / 876 | 204.0 / 243.0 / 310 / 207 / 2464 / 472 | 147.0 / 148.0 / 170 / 169 / 1864 / 412 | 233.0 / 303.5 / 235 / 11 / 2324 / 456 | 178.0 / 227.0 / 340 / 23 / 2664 / 500 | 205.0 / 246.0 / 186 / 20 / 1960 / 420 | - | 107.3 / 115.0 / 103 / 245 / 1872 / 404 |
| #8 registry, leanest valid configuration | 8.0 / 8.0 / 34 / 7 / 0 / 0 | 26.0 / 41.0 / 91 / 31 / 1256 / 144 | 86.0 / 79.0 / 200 / 23 / 1428 / 168 | 86.5 / 77.5 / 141 / 34 / 1432 / 148 | 336.0 / 599.0 / 2554 / 31 / 2072 / 612 | 114.0 / 107.0 / 256 / 40 / 1876 / 204 | 103.0 / 78.0 / 143 / 32 / 1416 / 152 | 137.5 / 136.5 / 195 / 23 / 1792 / 192 | 102.0 / 97.0 / 329 / 39 / 2732 / 240 | 143.0 / 141.0 / 186 / 20 / 1600 / 164 | - | 73.8 / 63.3 / 90 / 237 / 1420 / 140 |

</details>

### Publisher spelling

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written, runtime addresses | 1 | **2** ×1.27 | **3** ×1.00 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | 1/6 (vs `handwritten`; fails: no extra RAM 5, setup instr 3, publish instr 1) |
| hand-written, context + function pointer | 1 | **1** ×1.73 | **1** ×1.36 | **2** ×1.27 | **3** ×1.00 (1 indirect) | **1** ×1.57 | **1** ×2.00 | **3** | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, publish instr 4) |
| names the `StaticWiring` alias (alt6) | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (6/6; vs `handwritten`) |
| `template<class Out>` by hand (alt1) | 1 | **2** ×1.27 | **3** ×1.00 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| CRTP mixin `Publisher<Derived, Out>` (alt2) | 1 | **2** ×1.27 | **3** ×1.00 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| CTAD factory (alt3) | 1 | **2** ×1.27 | **2** ×1.12 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | 4/6 (vs `handwritten_runtime`; fails: setup instr 2, publish path 2, no extra RAM 2) |
| call-site output argument (alt4) | 1 | **2** ×1.27 | **3** ×1.00 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| `Sink<T>` (alt5) | 1 | **1** ×1.73 | **2** ×1.33 | **2** ×1.27 | **3** ×1.00 (1 indirect) | **1** ×1.57 | **1** ×2.00 | **3** | 5/6 (vs `handwritten_erased`; fails: publish path 1) |
| deducing-this mixin, C++23 (alt7) | 1 | - | **3** ×1.00 | - | - | - | - | **3** | **=** (2/2; vs `handwritten_runtime`) |
| deducing-this call site, C++23 (alt8) | 1 | - | **3** ×1.00 | - | - | - | - | **3** | **=** (2/2; vs `handwritten_runtime`) |

<details><summary>Publisher spelling: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `publisher_ergonomics` |
|---|---|
| hand-written, static | 30.0 / 33.0 / 37 / 30 / 92 / 12 |
| hand-written, runtime addresses | 38.0 / 33.0 / 43 / 32 / 104 / 24 |
| hand-written, context + function pointer | 52.0 / 45.0 / 47 / 15 / 144 / 32 |
| names the `StaticWiring` alias (alt6) | 30.0 / 33.0 / 37 / 30 / 92 / 12 |
| `template<class Out>` by hand (alt1) | 38.0 / 33.0 / 43 / 32 / 104 / 24 |
| CRTP mixin `Publisher<Derived, Out>` (alt2) | 38.0 / 33.0 / 43 / 32 / 104 / 24 |
| CTAD factory (alt3) | 38.0 / 37.0 / 43 / 32 / 108 / 24 |
| call-site output argument (alt4) | 38.0 / 33.0 / 43 / 32 / 104 / 24 |
| `Sink<T>` (alt5) | 52.0 / 44.0 / 47 / 15 / 144 / 32 |
| deducing-this mixin, C++23 (alt7) | - / 33.0 / - / - / - / - |
| deducing-this call site, C++23 (alt8) | - / 33.0 / - / - / - / - |

</details>

### Static-path cancellation

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static | 2 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written, runtime addresses | 1 | **2** ×1.16 | **3** ×1.00 | **2** ×1.17 | **3** ×1.09 | **2** ×1.23 | **1** ×1.50 | **3** | 1/6 (vs `handwritten`; fails: no extra RAM 4, setup instr 3, publish path 2) |
| `bool receive()` + `publishCancelable` (B2) | 2 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.05 | **3** ×1.00 | **3** ×1.00 | **3** | 10/12 (vs `handwritten`; fails: publish path 2) |
| `bool receive()` + `publishCancelable` (B1, new) | 1 | **2** ×1.14 | **3** ×1.00 | **2** ×1.17 | **3** ×1.09 | **2** ×1.23 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| `std::expected` return, C++23 (B2) | 1 | **3** ×1.00 | - | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (4/4; vs `handwritten`) |
| `Delivery&` token (B2) | 2 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.05 | **3** ×1.00 | **3** ×1.00 | **3** | 10/12 (vs `handwritten`; fails: publish path 2) |
| `cancel()` flag, static storage (B2) | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **3** | 4/6 (vs `handwritten`; fails: no extra RAM 2, publish path 1, no Sub0Pub retained 1) |
| `cancel()` flag, thread-local (B2) | 1 | **2** ×1.11 | **3** ×1.00 | **3** ×1.00 | **1** ×1.44 | **1** ×1.41 | **0** ×16.56 | **1** TLS | 4/6 (vs `handwritten`; fails: no extra RAM 2, no Sub0Pub retained 2, publish instr 1) |
| filter on a shared flag (B2, no cancel support) | 1 | **2** ×1.23 | **3** ×1.00 | **3** ×1.00 | **2** ×1.34 | **1** ×1.36 | **3** ×1.00 | **3** | 2/6 (vs `handwritten`; fails: no extra RAM 4, publish path 3, publish instr 2) |

<details><summary>Static-path cancellation: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `cancellation` | `cancellation_filtered` |
|---|---|---|
| hand-written, static | 27.0 / 28.7 / 36 / 32 / 88 / 8 | 26.8 / 28.8 / 36 / 36 / 96 / 8 |
| hand-written, runtime addresses | 31.3 / 28.7 / 42 / 35 / 108 / 24 | - |
| `bool receive()` + `publishCancelable` (B2) | 27.0 / 28.7 / 36 / 32 / 88 / 8 | 27.2 / 28.0 / 36 / 40 / 108 / 8 |
| `bool receive()` + `publishCancelable` (B1, new) | 30.7 / 28.7 / 42 / 35 / 108 / 24 | - |
| `std::expected` return, C++23 (B2) | 27.0 / - / 36 / 32 / 88 / 8 | - |
| `Delivery&` token (B2) | 27.0 / 28.7 / 36 / 32 / 88 / 8 | 27.2 / 28.0 / 36 / 40 / 108 / 8 |
| `cancel()` flag, static storage (B2) | 29.0 / 28.7 / 36 / 37 / 100 / 12 | - |
| `cancel()` flag, thread-local (B2) | 30.0 / 28.7 / 36 / 46 / 124 / 265 | - |
| filter on a shared flag (B2, no cancel support) | 33.3 / 28.7 / 37 / 43 / 120 / 12 | - |

</details>

### Static/dynamic bridge

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static + registry | 3 | **3** ×1.03 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written registry, empty (`handwritten_registry`) | 1 | **2** ×1.13 | **1** ×1.46 | **2** ×1.17 | **2** ×1.27 | **2** ×1.31 | **0** ×2.50 | **3** | 0/6 (vs `handwritten`; fails: publish path 6, no extra RAM 6, publish instr 4) |
| `DynamicPort<T, N>` | 3 | **3** ×1.04 | **2** ×1.13 | **3** ×1.05 | **3** ×1.08 | **3** ×1.09 | **1** ×1.36 | **3** | 10/18 (vs `handwritten`, `handwritten_registry`; fails: no Sub0Pub retained 8, publish path 1) |
| `DynamicPort<T, N>`, C++23 bind result | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | 2/6 (vs `handwritten`; fails: no Sub0Pub retained 4) |
| `BrokerPort<T>` to the #8 registry (lean) | 3 | **3** ×1.10 | **2** ×1.16 | **0** ×2.05 | **2** ×1.27 | **0** ×8.35 | **0** ×4.76 | **1** operator delete | 0/18 (vs `handwritten`, `handwritten_registry`; fails: no extra RAM 18, setup instr 12, teardown instr 12) |
| inverted: static wiring as one registry subscriber | 3 | **1** ×1.67 | **1** ×1.43 | **0** ×3.41 | **2** ×1.24 (1 indirect) | **0** ×9.94 | **0** ×5.12 | **1** operator delete | 0/18 (vs `handwritten`, `handwritten_registry`; fails: no extra RAM 18, no Sub0Pub retained 18, no extra dependencies 18) |

<details><summary>Static/dynamic bridge: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `static_dynamic_bridge` | `static_dynamic_bridge_empty` | `static_dynamic_bridge_churn` |
|---|---|---|---|
| hand-written, static + registry | 37.0 / 58.0 / 63 / 45 / 472 / 44 | 23.0 / 24.0 / 35 / 22 / 48 / 4 | 81.8 / 94.0 / 97 / 187 / 592 / 56 |
| hand-written registry, empty (`handwritten_registry`) | - | 26.0 / 35.0 / 41 / 28 / 84 / 40 | - |
| `DynamicPort<T, N>` | 37.0 / 58.0 / 63 / 45 / 472 / 44 | 26.0 / 35.0 / 41 / 28 / 84 / 40 | 75.0 / 94.0 / 97 / 187 / 592 / 56 |
| `DynamicPort<T, N>`, C++23 bind result | 37.0 / 58.0 / 63 / 45 / 472 / 44 | - | - |
| `BrokerPort<T>` to the #8 registry (lean) | 37.0 / 57.0 / 137 / 48 / 2480 / 160 | 27.0 / 35.0 / 63 / 28 / 1620 / 148 | 84.3 / 101.0 / 213 / 280 / 2592 / 180 |
| inverted: static wiring as one registry subscriber | 74.0 / 68.0 / 212 / 24 / 2624 / 172 | 33.0 / 50.0 / 138 / 42 / 2468 / 160 | 122.8 / 111.8 / 291 / 263 / 2712 / 192 |

</details>

### Transport endpoint and split horizon

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static | 2 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written, runtime addresses | 2 | **3** ×1.06 | **3** ×1.00 | **2** ×1.14 | **3** ×1.07 | **2** ×1.18 | **3** ×1.06 | **3** | 4/12 (vs `handwritten`; fails: no extra RAM 8, setup instr 4, publish path 4) |
| hand-written, context + function pointer | 1 | **1** ×1.67 | **1** ×1.48 | **2** ×1.24 | **3** ×1.00 (1 indirect) | **1** ×2.00 | **2** ×1.25 | **3** | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, publish instr 4) |
| B2 `StaticForward`, origin = the binding | 2 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (12/12; vs `handwritten`) |
| B2 `StaticForward`, origin = the transport (new) | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (6/6; vs `handwritten`) |
| B1 `Forward`, origin by type | 2 | **3** ×1.06 | **3** ×1.06 | **2** ×1.14 | **3** ×1.07 | **2** ×1.19 | **3** ×1.06 | **3** | 10/12 (vs `handwritten_runtime`; fails: publish instr 2, setup instr 2, publish path 2) |
| B1 `Forward`, origin = the transport object | 2 | **2** ×1.15 | **2** ×1.12 | **2** ×1.14 | **2** ×1.19 | **2** ×1.30 | **3** ×1.06 | **3** | 6/12 (vs `handwritten_runtime`; fails: publish path 6, publish instr 4, setup instr 2) |
| B3 `Sink<T>` + `Forward` | 1 | **1** ×1.67 | **1** ×1.48 | **2** ×1.24 | **3** ×1.00 (1 indirect) | **1** ×2.00 | **2** ×1.25 | **3** | 4/6 (vs `handwritten_erased`; fails: publish path 2) |
| #8 `Route`, default configuration | 1 | **0** ×7.59 | **0** ×8.48 | **0** ×5.47 | **3** ×1.00 (2 indirect) | **0** ×30.63 | **0** ×26.25 | **0** TLS, operator delete | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, no Sub0Pub retained 6) |
| #8 `Route`, leanest valid configuration | 1 | **0** ×5.29 | **0** ×4.86 | **0** ×5.47 | **3** ×1.00 (2 indirect) | **0** ×25.00 | **0** ×10.25 | **1** operator delete | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, no Sub0Pub retained 6) |

<details><summary>Transport endpoint and split horizon: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `transport_endpoint` | `transport_two_links` |
|---|---|---|
| hand-written, static | 27.0 / 29.0 / 34 / 25 / 52 / 0 | 40.0 / 40.0 / 36 / 37 / 104 / 8 |
| hand-written, runtime addresses | 27.0 / 29.0 / 38 / 25 / 72 / 12 | 45.0 / 40.0 / 42 / 42 / 128 / 24 |
| hand-written, context + function pointer | 45.0 / 43.0 / 42 / 25 / 128 / 20 | - |
| B2 `StaticForward`, origin = the binding | 27.0 / 29.0 / 34 / 25 / 52 / 0 | 40.0 / 40.0 / 36 / 37 / 104 / 8 |
| B2 `StaticForward`, origin = the transport (new) | 27.0 / 29.0 / 34 / 25 / 52 / 0 | - |
| B1 `Forward`, origin by type | 27.0 / 29.0 / 38 / 25 / 72 / 12 | 45.0 / 45.0 / 42 / 42 / 132 / 24 |
| B1 `Forward`, origin = the transport object | 27.0 / 29.0 / 38 / 25 / 72 / 12 | 53.0 / 50.0 / 42 / 52 / 156 / 24 |
| B3 `Sink<T>` + `Forward` | 45.0 / 43.0 / 42 / 25 / 128 / 20 | - |
| #8 `Route`, default configuration | 205.0 / 246.0 / 186 / 20 / 1960 / 420 | - |
| #8 `Route`, leanest valid configuration | 143.0 / 141.0 / 186 / 20 / 1600 / 164 | - |

</details>

### Domains and sessions

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written, runtime addresses | 1 | **2** ×1.27 | **3** ×1.00 | **2** ×1.16 | **2** ×1.23 | **2** ×1.35 | **1** ×1.50 | **3** | 0/6 (vs `handwritten`; fails: no extra RAM 5, setup instr 4, publish path 3) |
| hand-written, context + function pointer | 1 | **0** ×2.17 | **1** ×1.71 | **1** ×1.38 | **2** ×1.17 (2 indirect) | **0** ×2.22 | **0** ×2.50 | **3** | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, publish instr 4) |
| hand-written, one gateway holding both domains' addresses (new) | 1 | **2** ×1.23 | **3** ×1.00 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | 1/6 (vs `handwritten`; fails: no extra RAM 5, setup instr 3, publish instr 1) |
| B2: one `StaticWiring` per domain | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (6/6; vs `handwritten`) |
| B2: one publisher naming both wirings (new) | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (6/6; vs `handwritten`) |
| B1: one `wire(...)` per domain | 1 | **2** ×1.27 | **3** ×1.00 | **2** ×1.16 | **2** ×1.23 | **2** ×1.35 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| B1: one publisher holding both wirings (new) | 1 | **2** ×1.23 | **3** ×1.09 | **2** ×1.16 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | 5/6 (vs `handwritten_gateway`; fails: publish instr 1, setup instr 1, publish path 1) |
| B3: one `Sink<T>` per domain (new) | 1 | **0** ×2.17 | **1** ×1.71 | **1** ×1.38 | **2** ×1.17 (2 indirect) | **0** ×2.22 | **0** ×2.50 | **3** | 5/6 (vs `handwritten_erased`; fails: publish path 1) |
| #8 `Domain`, default configuration | 1 | **0** ×5.93 | **0** ×6.67 | **0** ×9.19 | **3** ×1.00 (2 indirect) | **0** ×28.96 | **0** ×31.25 | **0** TLS, operator delete | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, no Sub0Pub retained 6) |
| #8 `Domain`, leanest valid configuration | 1 | **0** ×3.40 | **0** ×2.85 | **0** ×8.89 | **3** ×1.00 (1 indirect) | **0** ×29.70 | **0** ×15.00 | **1** operator delete | 0/6 (vs `handwritten`; fails: publish path 6, no extra indirect calls 6, no extra RAM 6) |

<details><summary>Domains and sessions: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `two_domains` |
|---|---|
| hand-written, static | 31.0 / 34.0 / 37 / 30 / 92 / 12 |
| hand-written, runtime addresses | 38.0 / 34.0 / 43 / 37 / 124 / 24 |
| hand-written, context + function pointer | 65.0 / 58.0 / 51 / 27 / 204 / 40 |
| hand-written, one gateway holding both domains' addresses (new) | 37.0 / 34.0 / 43 / 32 / 104 / 24 |
| B2: one `StaticWiring` per domain | 31.0 / 34.0 / 37 / 30 / 92 / 12 |
| B2: one publisher naming both wirings (new) | 30.0 / 34.0 / 37 / 30 / 92 / 12 |
| B1: one `wire(...)` per domain | 38.0 / 34.0 / 43 / 37 / 124 / 24 |
| B1: one publisher holding both wirings (new) | 37.0 / 37.0 / 43 / 32 / 104 / 24 |
| B3: one `Sink<T>` per domain (new) | 65.0 / 58.0 / 51 / 27 / 204 / 40 |
| #8 `Domain`, default configuration | 178.0 / 227.0 / 340 / 23 / 2664 / 500 |
| #8 `Domain`, leanest valid configuration | 102.0 / 97.0 / 329 / 39 / 2732 / 240 |

</details>

### Cross-TU and LTO

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| hand-written, static, no LTO | 1 | **0** ×2.03 | **1** ×1.77 | **2** ×1.16 | **1** ×1.54 | **1** ×1.63 | **3** ×1.00 | **3** | reference |
| hand-written, static, LTO | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | reference |
| hand-written, runtime addresses, no LTO | 1 | **0** ×2.03 | **1** ×1.77 | **2** ×1.34 | **1** ×1.50 | **1** ×1.68 | **1** ×1.50 | **3** | 0/6 (vs `handwritten`; fails: no extra RAM 6, setup instr 4) |
| hand-written, runtime addresses, LTO | 1 | **3** ×1.10 | **3** ×1.00 | **2** ×1.19 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | 1/6 (vs `handwritten`; fails: no extra RAM 5, setup instr 3, publish instr 1) |
| hand-written, context + function pointer, no LTO (new) | 1 | **0** ×2.47 | **0** ×2.16 | **1** ×1.47 | **3** ×1.00 (1 indirect) | **0** ×2.21 | **1** ×2.00 | **3** | 0/6 (vs `handwritten`; fails: no extra indirect calls 6, no extra RAM 6, publish instr 4) |
| hand-written, context + function pointer, LTO (new) | 1 | **1** ×1.60 | **2** ×1.32 | **2** ×1.31 | **3** ×1.00 (1 indirect) | **1** ×1.58 | **1** ×2.00 | **3** | 0/6 (vs `handwritten`; fails: no extra RAM 6, no extra indirect calls 5, publish instr 4) |
| B2, no LTO | 1 | **0** ×2.03 | **1** ×1.77 | **2** ×1.16 | **1** ×1.54 | **1** ×1.63 | **3** ×1.00 | **3** | **=** (6/6; vs `handwritten`) |
| B2, LTO | 1 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | **=** (6/6; vs `handwritten`) |
| B1, no LTO | 1 | **0** ×2.03 | **1** ×1.77 | **2** ×1.34 | **1** ×1.50 | **1** ×1.68 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| B1, LTO | 1 | **3** ×1.10 | **3** ×1.00 | **2** ×1.19 | **3** ×1.00 | **3** ×1.00 | **1** ×1.50 | **3** | **=** (6/6; vs `handwritten_runtime`) |
| B3, no LTO (new) | 1 | **0** ×2.47 | **0** ×2.13 | **1** ×1.47 | **3** ×1.00 (1 indirect) | **0** ×2.21 | **1** ×2.00 | **3** | **=** (6/6; vs `handwritten_erased`) |
| B3, LTO (new) | 1 | **1** ×1.60 | **2** ×1.29 | **2** ×1.31 | **3** ×1.00 (1 indirect) | **1** ×1.58 | **1** ×2.00 | **3** | 5/6 (vs `handwritten_erased`; fails: publish path 1) |
| A default, no LTO | 1 | **0** ×4.33 | **0** ×4.84 | **0** ×5.75 | **0** ×3.56 (2 indirect) | **0** ×25.37 | **0** ×27.50 | **0** TLS, operator delete | 0/6 (vs `handwritten`; fails: publish path 6, no extra indirect calls 6, no extra RAM 6) |
| A default, LTO | 1 | **0** ×4.33 | **0** ×4.71 | **0** ×5.75 | **0** ×3.36 (2 indirect) | **0** ×24.53 | **0** ×27.75 | **0** TLS, operator delete | 0/6 (vs `handwritten`; fails: publish path 6, no extra indirect calls 6, no extra RAM 6) |
| A leanest, no LTO | 1 | **0** ×3.80 | **0** ×4.29 | **0** ×5.75 | **2** ×1.16 (2 indirect) | **0** ×21.16 | **0** ×27.50 | **0** TLS, operator delete | 0/6 (vs `handwritten`; fails: publish path 6, no extra indirect calls 6, no extra RAM 6) |
| A leanest, LTO | 1 | **0** ×3.80 | **0** ×4.16 | **0** ×5.75 | **3** ×1.00 (2 indirect) | **0** ×20.37 | **0** ×27.75 | **0** TLS, operator delete | 0/6 (vs `handwritten`; fails: publish path 6, no extra indirect calls 6, no extra RAM 6) |

<details><summary>Cross-TU and LTO: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `cross_file` |
|---|---|
| hand-written, static, no LTO | 61.0 / 55.0 / 37 / 43 / 124 / 12 |
| hand-written, static, LTO | 30.0 / 31.0 / 32 / 30 / 76 / 12 |
| hand-written, runtime addresses, no LTO | 61.0 / 55.0 / 43 / 42 / 128 / 24 |
| hand-written, runtime addresses, LTO | 33.0 / 31.0 / 38 / 28 / 80 / 24 |
| hand-written, context + function pointer, no LTO (new) | 74.0 / 67.0 / 47 / 17 / 168 / 32 |
| hand-written, context + function pointer, LTO (new) | 48.0 / 41.0 / 42 / 15 / 120 / 32 |
| B2, no LTO | 61.0 / 55.0 / 37 / 43 / 124 / 12 |
| B2, LTO | 30.0 / 31.0 / 32 / 30 / 76 / 12 |
| B1, no LTO | 61.0 / 55.0 / 43 / 42 / 128 / 24 |
| B1, LTO | 33.0 / 31.0 / 38 / 28 / 80 / 24 |
| B3, no LTO (new) | 74.0 / 66.0 / 47 / 17 / 168 / 32 |
| B3, LTO (new) | 48.0 / 40.0 / 42 / 15 / 120 / 32 |
| A default, no LTO | 130.0 / 150.0 / 184 / 178 / 1928 / 440 |
| A default, LTO | 130.0 / 146.0 / 184 / 168 / 1864 / 444 |
| A leanest, no LTO | 114.0 / 133.0 / 184 / 58 / 1608 / 440 |
| A leanest, LTO | 114.0 / 129.0 / 184 / 50 / 1548 / 444 |

</details>

### C++ standard of the spelling

| Option | cases | gcc publish | clang publish | setup+teardown | cm33 path | cm33 text | cm33 RAM | cm33 deps | vs its equal-work reference |
|---|---|---|---|---|---|---|---|---|---|
| C++17 spelling | 5 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | 26/30 (vs `handwritten`, `handwritten_runtime`; fails: no Sub0Pub retained 4) |
| C++20/23 spelling (concepts, `std::expected`, deducing this) | 5 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** ×1.00 | **3** | 20/24 (vs `handwritten`, `handwritten_runtime`; fails: no Sub0Pub retained 4) |

<details><summary>C++ standard of the spelling: raw values per case (gcc publish / clang publish / setup+teardown / cm33 path / cm33 app text / cm33 app RAM)</summary>

| Option | `filters` | `multi_receivers` | `cancellation` | `static_dynamic_bridge` | `publisher_ergonomics` |
|---|---|---|---|---|---|
| C++17 spelling | 19.0 / 22.0 / 34 / 19 / 32 / 0 | 30.0 / 33.0 / 37 / 30 / 92 / 12 | 27.0 / 28.7 / 36 / 32 / 88 / 8 | 37.0 / 58.0 / 63 / 45 / 472 / 44 | 38.0 / 33.0 / 43 / 32 / 104 / 24 |
| C++20/23 spelling (concepts, `std::expected`, deducing this) | 19.0 / 22.0 / 34 / 19 / 32 / 0 | 30.0 / 33.0 / 37 / 30 / 92 / 12 | 27.0 / - / 36 / 32 / 88 / 8 | 37.0 / 58.0 / 63 / 45 / 472 / 44 | - / 33.0 / - / - / - / - |

</details>

### gcc and clang disagreements

Variants whose verdict against their equal-work reference differs between gcc-O2 and clang-O2.

| Case | Variant | Form | gcc-O2 | clang-O2 |
|---|---|---|---|---|
| `cancellation_filtered` | `sub0x_alt1_bool` | observable | publish path (publish +0.3) | PASS (publish -0.8) |
| `cancellation_filtered` | `sub0x_alt2_token` | observable | publish path (publish +0.3) | PASS (publish -0.8) |
| `cancellation` | `sub0x_alt3_static` | observable | no extra RAM (publish +2.0) | PASS (publish +0.0) |
| `cancellation` | `sub0x_alt3_tls` | observable | no Sub0Pub retained, no extra RAM, publish instr (publish +3.0) | PASS (publish +0.0) |
| `cancellation` | `sub0x_alt4_filter` | observable | no extra RAM, publish instr, publish path, setup instr (publish +6.3) | PASS (publish +0.0) |
| `cancellation` | `sub0x_alt4_filter` | removable | no extra RAM, publish instr, setup instr (publish +3.0) | PASS (publish +0.0) |
| `filters` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish -1.0) |
| `large_payload` | `sub0x_b3_sink` | observable | PASS (publish -1.0) | publish path (publish -1.0) |
| `many_receivers` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish -1.0) |
| `multi_receivers` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish -1.0) |
| `multi_types` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish +0.0) |
| `nested_publish` | `sub0x_b1_wire` | observable | no Sub0Pub retained, publish path (publish -1.5) | PASS (publish +0.0) |
| `one_receiver` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish -1.0) |
| `publisher_ergonomics` | `alt3_ctad_factory` | observable | PASS (publish +0.0) | no extra RAM, publish instr, publish path, setup instr (publish +4.0) |
| `publisher_ergonomics` | `alt3_ctad_factory` | removable | PASS (publish +0.0) | no extra RAM, publish path, setup instr (publish +1.0) |
| `publisher_ergonomics` | `alt5_sink_typeerased` | observable | PASS (publish +0.0) | publish path (publish -1.0) |
| `transport_endpoint` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish +0.0) |
| `transport_endpoint` | `sub0x_b3_sink` | removable | PASS (publish +0.0) | publish path (publish +0.0) |
| `transport_two_links` | `sub0x_b1_wire_typed_links` | observable | PASS (publish +0.0) | no extra RAM, publish instr, publish path, setup instr (publish +5.0) |
| `transport_two_links` | `sub0x_b1_wire_typed_links` | removable | PASS (publish +0.0) | no extra RAM, publish instr, publish path, setup instr (publish +4.0) |
| `two_domains` | `sub0x_b1_one_publisher` | observable | PASS (publish +0.0) | no extra RAM, publish instr, publish path, setup instr (publish +3.0) |
| `two_domains` | `sub0x_b3_sink` | observable | PASS (publish +0.0) | publish path (publish +0.0) |


## Guarantees and capabilities

✓ is demonstrated by the cited case (behaviour-checked by ctest on every CI compiler) or test; ✗ is a compile error
(cited) or a demonstrated limitation; "–" does not apply. Cases are `tests/collapse/cases/<case>/<variant>`; unit tests
are in `tests/collapse/unit/test_sub0x_sandbox.cpp`.

| Option | Dynamic object lifetimes | Runtime subscribe / unsubscribe | Publisher behind a library or ABI boundary | Receiver stops a publication | Split horizon | Isolated sessions of one type | Nested and re-entrant publish | Misuse caught at compile time | Silent capability mismatch (K14) |
|---|---|---|---|---|---|---|---|---|---|
| **B2 `StaticWiring`** | ✗ static storage, complete objects only (`cf_static_wiring_local`, `cf_static_wiring_element`; K20) | ✗ fixed at compile time; add a `DynamicPort` binding | ✗ the publisher names the wiring or is a template | ✓ bool, token, flags (`cancellation`, `cancellation_filtered`) | ✓ decided at compile time, even for two same-typed transports (`transport_endpoint`, `transport_two_links`) | ✓ one wiring per session (`two_domains`) | ✓ `nested_publish`, unit "nested publish" | ambiguous origin (`cf_publish_from_ambiguous`), opt-in `handles_v` (`cf_handles_mismatch`) | hazard, opt-in guard `sub0x::handles_v` (unit "limitation K14") |
| **B1 `wire(...)`** | ✓ bindings are runtime addresses (every B1 variant binds in `collapse_setup`) | ✗ fixed when wired; `DynamicPort` binding | ✗ the publisher is a template over `Out` | ✓ bool (`publishCancelable`, new: `cancellation/sub0x_alt1_bool_b1`, unit tests) | ✓ by type at compile time; same-typed endpoints by a runtime compare (`transport_two_links`, K18) | ✓ (`two_domains`) | ✓ `nested_publish` | as B2 (ambiguous origin, `handles_v`) | hazard, opt-in guard |
| **B3 `Sink<T>`** | ✓ (as B1) | ✗ | ✓ non-template publisher in its own TU (`cross_file/sub0x_b3_sink/sensor.cpp`) | ✗ `Sink` has no cancelable publish (K19) | ✓ ingress through the wiring (`transport_endpoint/sub0x_b3_sink`) | ✓ (`two_domains/sub0x_b3_sink`) | – (as the wiring behind it) | as B1 behind the port | hazard, opt-in guard |
| **A today's API** | ✓ | ✓ (`dynamic_subscriptions`) | ✓ virtual `Subscribe<T>` | ✓ `cancel()` (`tests/test_cancel.cpp`) | ✗ ingress would echo (COLLAPSE_EVIDENCE.md) | ✗ one table per type | ✓ with the snapshot (`nested_publish`; `SUB0PUB_REENTRANT_SAFE=false` forbids it) | runtime only | none: `override` checks the signature |
| **#8 registry** | ✓ | ✓ (`dynamic_subscriptions`) | ✓ | ✓ with a context (`test_axes`) | ✓ `Route` (`transport_endpoint`) | ✓ `Domain` (`two_domains`) | ✓ Snapshot and Direct (`nested_publish`) | 8 compile-fail checks (`tests/design/broker_config/compile_fail`) | none |
| **`DynamicPort<T, N>` bridge** | ✓ | ✓ (`static_dynamic_bridge_churn`) | – | ✗ dynamic receivers cannot stop | – | one port per wiring | ✗ removal during delivery skips the next receiver (unit "limitation K21"); `add()` at capacity drops silently (unit, `tryAdd` reports) | – | – |
| **`BrokerPort<T>` bridge** | ✓ | ✓ (`static_dynamic_bridge_churn`) | ✓ | per #8 configuration | per #8 | ✓ `Domain` | ✓ with Snapshot: self-removal does not skip the next (unit "BrokerPort with Snapshot") | per #8 | none |

Two further hazards the review pinned with tests: a receiver that returns `false` to stop a publication is ignored by
plain `publish()` (unit "cancellation", K19), and binding an array object binds nothing (unit "limitation K14/K20").
Concurrency of the static paths is not tested: B1/B2 dispatch keeps no shared state, so concurrent publishers are safe
exactly when the receivers are, but no TSan test demonstrates it (open).

## Reading the matrix

- **Binding form.** B2 is the only form that scores 3 on every cost metric, and it is `=` against static hand-written
  code in 64 of 66 case × build × form checks. The two misses are both `nested_publish` on one compiler (K22): gcc
  keeps an out-of-line recursive copy of the publication (+536 B text on x86, publish -1.5 instructions), clang
  spends +4 instructions. B1 costs what runtime binding costs (gcc publish ×1.09, RAM ×1.20, the same as
  `handwritten_runtime`) and is `=` against that reference in 65 of 66. B3 costs what type erasure costs (gcc ×1.74,
  the same as `handwritten_erased`) and is `=` on gcc and Cortex-M33; on clang 9 of its checks fail only the static
  path criterion, because clang inlines the call through the erased pointer (publish -1 to +0 instructions). Pattern A (×2.3 to ×3.0 gcc publish, ×13 to ×15 application text) and the
  #8 registry (×2.3 lean, ×4.0 default) score 0 on almost everything: they are runtime registries, priced here in
  static scenarios on purpose.
- **Fan-out has no loop form.** At 32 receivers of one type B2 is exactly the unrolled hand-written code (202 gcc /
  256 clang instructions), but a hand-written loop over an array is 668 B smaller on Cortex-M33 (and faster on clang,
  230, slower on gcc, 332). Pattern B cannot express that loop: a StaticWiring cannot bind array elements (C++17) and
  a bound array binds nothing (K20).
- **Publisher spelling.** Naming the `StaticWiring` alias is the only spelling that scores 3 everywhere; the
  template, CRTP mixin, call-site and deducing-this spellings all equal hand-written runtime binding, and only the CTAD
  factory adds cost (clang +4). Unchanged from the spike's decision.
- **Cancellation.** `bool receive()` and the token stay 3 on every metric, but not exactly `=` when combined with
  filters: Cortex-M33 +4 path instructions and +12 B (`cancellation_filtered`, K24). The flags cost RAM, TLS or
  retained state as the spike found. B1 now has `publishCancelable` and it is `=` against hand-written runtime code.
- **Bridge.** `DynamicPort` matches the hand-written registry on publish, setup, teardown and RAM with one, several,
  churning or no dynamic subscribers, and on Cortex-M33 in every respect. On x86, gcc and clang keep an out-of-line
  copy of `DynamicPort::receive` beside the inlined one when subscribers exist (+48 / +30 B text, +68 B with churn,
  where gcc's path is also +15 instructions while its publish is 6.8 instructions cheaper), which the corrected
  retained-code accounting now reports (K21). `BrokerPort` pays setup/teardown ×2 and ×8 text;
  the inverted bridge costs more still, as the spike found.
- **Transport.** Naming the transport object as the ingress origin now works (it echoed before, a defect fixed
  here), and costs nothing in B2. In B1 it costs an address compare per same-typed endpoint: +8 gcc, +10 clang
  publish, +10 Cortex-M33 path with two links of one type (K18); one-line per-link adapter types restore `=` on gcc and
  Cortex-M33 (clang +5, K23).
- **Domains.** One wiring per session is `=` in every form; one publisher holding both wirings is `=` in B2 and on
  gcc/Cortex-M33 in B1 (clang +3 publish, +24 B RAM against a hand-written gateway, K23).
- **Cross-TU.** Without LTO every form pays the out-of-line call, identically to its hand-written reference; with LTO
  B2 is the cheapest option, B1 equals its reference, and B3 does too apart from clang's inlined static path. LTO does not rescue pattern A.
- **C++ standard.** The C++20/23 spellings are byte-identical in cost to their C++17 counterparts wherever they build.

## Are the use cases exhaustive? (coverage review)

Each option's merits and limitations need a case or test that exercises them. Before this review, and after:

| Scenario | Before | Now |
|---|---|---|
| Fan-out scaling | 3 receivers at most | `many_receivers`: 32 receivers, every binding form, and `handwritten_loop` pricing the missing loop form |
| Several message types in one wiring; a receiver handling several types | none | `multi_types`: capability routing across two types, every binding form |
| Nested and re-entrant publish on the static path | none (AXIS_SCORES.md covers the #8 registry only) | `nested_publish` (another type, and the same type once) in every binding form; unit test for B2 |
| One publisher feeding several wirings | none | `two_domains/sub0x_b1_one_publisher`, `sub0x_b2_one_publisher`, with an equal-layout `handwritten_gateway` reference |
| Filters that reject | covered (`filters`: EvenMonitor) | unchanged |
| Large payloads | none (4-byte messages) | `large_payload`: a 64-byte frame, every binding form |
| B3 across translation units, with and without LTO | not measured | `cross_file/sub0x_b3_sink` (publisher in its own TU) and `handwritten_erased` |
| B3 for two domains and transport endpoints | not measured | `two_domains/sub0x_b3_sink`, `transport_endpoint/sub0x_b3_sink`, each with `handwritten_erased` |
| #8 registry in the core static cases | 3 cases | 11 cases (default and leanest valid configuration), so the binding-form axis is square |
| Cancellation combined with filters | none | `cancellation_filtered` (bool and token), unit test |
| Cancellation on a runtime-bound wiring | not expressible (StaticWiring only) | `Wiring::publishCancelable` added; `cancellation/sub0x_alt1_bool_b1` with `handwritten_runtime` |
| Bridge with several dynamic subscribers and churn | one subscriber, subscribed once | `static_dynamic_bridge_churn`: two resident, one transient every fourth publication |
| Dynamic subscriber removing itself during delivery | none | unit tests: `DynamicPort` skips the next receiver (limitation), `BrokerPort` with Snapshot does not |
| Split horizon, origin named by the transport | none | `transport_endpoint/*_origin_transport` (found a defect), unit test |
| Two endpoints of one transport type | none | `transport_two_links`: B2, B1 by object, B1 with per-link adapter types |
| K14 silent capability mismatch | prose only | unit test (wrong parameter, const binding), opt-in `sub0x::handles_v` guard, `cf_handles_mismatch` |
| What B2 cannot express | prose only | `cf_static_wiring_local`, `cf_static_wiring_element`, `cf_publish_from_ambiguous`; unit test for a bound array |
| Cortex-M33 with LTO | `cross_file` only | unchanged (LTO builds run for multi-TU cases only), now including B3 |
| gcc and clang disagreements | noted case by case | generated table (above) |
| Harness: broken builds, missing references, empty selections | exit 0 | exit non-zero; feature-gated variants reported as skipped |

**Fairness re-check.** Every new variant declares its equal-work reference, and the earlier fairness review's rules
were re-applied: B1 variants hold the wiring by value and are judged against `handwritten_runtime`, B3 against
`handwritten_erased`, pattern A and the #8 registry in their leanest valid configurations beside their defaults
(pattern A in `nested_publish` cannot drop the snapshot: `SUB0PUB_REENTRANT_SAFE=false` forbids re-entrant publish;
the #8 lean `filters` variant keeps `filter()`, which the case uses). One new comparison was unfair on first
measurement and was corrected: the one-publisher B1 variant was compared with `handwritten_runtime` (two publisher
objects); `handwritten_gateway` has its layout. Two avenues were tried and did not remove the clang B1 cost (K23):
storing pointers instead of references in `Wiring` changed nothing, so the cost is not the reference member.

## Defects found

1. **Split horizon echoed when ingress named the transport** (sandbox, fixed). `publishFrom(radio, msg)` on a wiring
   that binds `Forward<Radio>` or `StaticForward<&radio>` compared the origin's type with the adapter's, never
   matched, and sent the ingress straight back out through the radio. Demonstrated: the new variants
   `transport_endpoint/sub0x_b1_wire_origin_transport` and `sub0x_b2_static_origin_transport` built against the
   2ead704 header print `io=3367166808` where the reference prints `io=1953632812` (a ctest behaviour failure).
   Fixed: adapters declare `sub0x_endpoint`, and `StaticForward` an identity, so the transport and its adapter are the
   same origin (`detail::isOrigin`). Existing variants' measurements are unchanged (identical JSON on every build).
2. **The evidence tool passed broken runs** (tool, fixed). Build errors were printed as table rows and only checksum
   differences failed the run: at 2ead704, a case whose reference and variant both fail to compile, and an empty
   `--case` selection, both exit 0 (demonstrated with an injected case). Now build, run and callgrind failures,
   missing references, and empty case or build selections exit non-zero; variants marked `// SUB0X_REQUIRES:` are
   probed per build (the same `deducing-this` and `expected` probes as CMake) and reported as skipped. The 10
   "build error" rows of every earlier full run were exactly these skips.
3. **Publish-path call targets were truncated at the first `>` or `+`** (tool, fixed). A call to
   `sub0::detail::Broker<app::Sample>::publish(...)` or `void sub0x::detail::deliver<R, M>(R&, M const&)` became an
   unresolvable name, so the callee's instructions were left off the path, and a PLT stub reached by a tail jump was
   followed as if it were code. Targets are now resolved by address from objdump's numeric operand; `--self-test`
   (ctest `Sub0Pub_CollapseEvidenceSelfTest`) checks nested-template callees, an offset-suffixed tail call, a PLT stub,
   an indirect call and an Arm tail call to `operator>>`.
4. **Retained `sub0x::` code was invisible** (tool, fixed). The retained-library count matched `sub0::` only, so the
   #8 prototype and the sandbox always reported 0 bytes. Both are now counted and shown separately (`sub0/sub0x`).
   Sandbox code that survives as a named function passes the criterion only if the image is no larger than the
   reference's (the same code under another name, as B3's erasure thunk is).

**Published numbers and verdicts changed by fixes 3 and 4** (full list:
[../perf/collapse/phase1-scores-2026-09-tool-fixes.md](../perf/collapse/phase1-scores-2026-09-tool-fixes.md)):

| Where | Before | After |
|---|---|---|
| Pattern A, `cross_file`, Cortex-M33 publish path (COLLAPSE_EVIDENCE.md: "work behind an indirect call appears as an indirect-call count") | 14 instructions (-29 against hand-written), 1 direct / 0 indirect calls | 178 (+135) default, 58 (+15) leanest; 5 direct / 2 indirect: `Broker::publish` was cut off the path |
| Pattern A and #8 registry, `dynamic_subscriptions`, path | e.g. Cortex-M33 A 46, #8 35; clang A 129, #8 152 | Cortex-M33 A 203, #8 245; clang A 215, #8 288 (gcc A 215 → 212: a PLT stub no longer counted) |
| `DynamicPort` bridge (spikes/README.md: "`=` ... on every build") | PASS on gcc and clang | publish, setup, teardown, path and RAM still `=`; image +48 B (gcc) / +30 B (clang) text from a retained out-of-line `DynamicPort::receive`, so "no Sub0Pub retained" fails on x86; Cortex-M33 unchanged `=` |
| B3 `Sink<T>` | retained 0 B | retained 2 to 99 B (the erasure thunk and its lambda), image equal to `handwritten_erased`: verdict unchanged |
| #8 registry variants | retained 0 B | retained hundreds of bytes (316 to 763 B in `dynamic_subscriptions`); they failed other criteria already |
| Cancellation flags (`sub0x_alt3_*`) | retained 0 B | 1 B (`g_canceled`); both flags now also fail "no Sub0Pub retained" on Cortex-M33, the thread-local one also on gcc (both were rejected already) |

No conclusion of the spike decisions is reversed: every changed verdict belongs to an option that was already
rejected or already carried a known cost, except the x86 image cost of `DynamicPort` (new K21), which does not
change the bridge decision (the alternatives cost kilobytes).

## Open gaps

- **MSVC** (`dumpbin`) and **RISC-V** evidence: unchanged from COLLAPSE_EVIDENCE.md's plan.
- **Static-path concurrency coverage:** `unit/test_concurrent_wiring.cpp` exercises shared B1/B2 wiring and
  B3 `Sink` from four threads, checking delivery, checksums and bool-cancellation isolation under TSan.
  Bindings stay immutable and receivers supply synchronization. `DynamicPort` remains single-threaded;
  concurrent registry mutation is outside its contract.
- **Root causes, inferred not proven**: the clang B1 cost in aggregates (K23) and gcc's out-of-line recursive
  publication (K22) are optimiser decisions; the review measured them and ruled out the reference member, nothing more.
- **C++23 deducing-this spellings** are measured on clang only (GCC 13 lacks the feature); `std::expected` on gcc only.
- **Fan-out** is measured at 3 and 32 receivers of one type; heterogeneous large fan-out and the size at which a loop
  form would pay for itself are not.
- **The retained-code criterion for `sub0x::` is a heuristic** (image no larger than the reference's); it cannot tell
  a renamed hand-written function from a small added one of equal size elsewhere.
- **ISR publication, deferred dispatch, target-level contention**: as in AXIS_SCORES.md.

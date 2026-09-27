# Response to the local review of #10 and #8 (2026-09-26)

The review was of #10 at `fab8891` and #8 at `af1e5dd`, and it made no remote writes. Every finding was reproduced
before it was fixed, and each fix carries a regression test. Fixes land on the #10 branch
(`claude/sub0pub-issue9-collapse`), which merges into #8.

| # | Finding | Status | Commit | Evidence |
|---|---|---|---|---|
| P1 | #10: quiescence slot ownership race (`claimed` set by CAS before a non-atomic `ownerId` is written; slot reuse races) | **Fixed.** Ownership is one atomic word (0 = free, else a per-thread token); claim is a CAS, release stores 0, and readers make one acquire load | `2ead704` | Previous code exits 66 under GCC 13 TSan with races at `qx_refcount.hpp:116` and `qx_epoch.hpp:85`; fixed code clean in 5 of 5 runs. Lifetime probes (nested publish, slot reuse) pass. Re-measured: [spikes/quiescence.md](spikes/quiescence.md) section 12 |
| P2 | #10: broken measurement builds return success | **Fixed.** Build, run and callgrind failures, missing declared references and empty selections exit non-zero; `SUB0X_REQUIRES` variants are probed per build (same probes as CMake) and reported as skipped. Cost verdicts stay report-only, by design | `6bdf5fe` | A broken reference exits 1 and an empty selection exits 2 (both exited 0 before). [COLLAPSE_SCORES.md](COLLAPSE_SCORES.md) "Defects found" |
| P2 | #10: templated call targets truncated; retained `sub0x::` code not counted | **Fixed.** Call and tail-call targets are resolved by address from objdump's numeric operand; functions are keyed by address; PLT stubs are not followed. `sub0x::` retention is reported separately from `sub0::` | `6bdf5fe` | `collapse_evidence.py --self-test` (ctest `Sub0Pub_CollapseEvidenceSelfTest`): nested-template callee, tail call, PLT stub, indirect call, Arm `operator>>`. Changed numbers, before and after on the same sources: [../perf/collapse/phase1-scores-2026-09-tool-fixes.md](../perf/collapse/phase1-scores-2026-09-tool-fixes.md). No spike decision is reversed |
| P1 | #8 (inherited by #10): Snapshot + NoContext use-after-free | **Fixed.** Snapshot requires a publish context (compile error `cf_snapshot_no_context`); Snapshot's guarantee depended on the context axis without saying so | `2ead704` | Reproduced under ASan (heap-use-after-free). `test_axes`: a later subscriber destroyed by an earlier receiver is not called (ASan clean). [AXIS_SCORES.md](AXIS_SCORES.md) updated |
| P1 | #8 (inherited by #10): delivery continues after a disconnect inside `filter()`; self-destruction in `filter()` calls a freed object | **Fixed.** `kit::deliverAt` re-reads the dispatch-owned slot, never the subscriber, after `filter()`. The NoFilter path is unchanged, and Direct dispatch skips the null check | `2ead704` | Reproduced: `receive()` called once after disconnect. `test_axes` covers disconnect, self-destruction and domain close from `filter()`; removing the re-check makes ASan fail. Measured cost: none (Lean 9/33/96 and Direct 61/166 unchanged) |
| – | #10: two generated test ELF files lacked executable bits in the local checkout | Not reproduced in CI or here; a property of that checkout, not of the build | – | CI runs every collapse ctest on Linux, macOS and Windows |

## Found by the score-matrix work in the same round

- **Lock + StaticContext** let one publisher thread cancel another's publication (lost deliveries, TSan races). A Lock
  now requires `ThreadLocalContext` (`cf_lock_static_context`). Commit `fab8891`; [AXIS_SCORES.md](AXIS_SCORES.md).
- **Split horizon echo in the pattern B sandbox:** `publishFrom(radio, msg)` on a wiring binding `Forward<Radio>` or
  `StaticForward<&radio>` sent the ingress back out through the radio (io checksum 3367166808 against the reference's
  1953632812). Fixed with an endpoint trait and identity. Commit `6bdf5fe`; [COLLAPSE_SCORES.md](COLLAPSE_SCORES.md).

## Where to start

1. [AXIS_SCORES.md](AXIS_SCORES.md): #8's policy axes, scored option by option, with guarantees and coverage.
2. [COLLAPSE_SCORES.md](COLLAPSE_SCORES.md): #10's axes (binding form, publisher spelling, cancellation, bridge,
   transport, domains, cross-TU/LTO, C++ standard), scored the same way.
3. [spikes/README.md](spikes/README.md): the decision record.
4. [BROKER_CUSTOMISATION.md](BROKER_CUSTOMISATION.md) section 8: known issues K1 to K24, each with a measured cost.

Still open, and listed in both score documents: concurrency tests for the static paths, root causes of K22 to K24,
MSVC and RISC-V evidence, ISR publish, and lock cost on the target.

## Consolidation review (2026-09-27)

Re-reviewed `384f164` against all five original findings. Atomic ownership, the Snapshot/context constraint,
dispatch-slot revalidation, strict build gating and address-based disassembly traversal address those findings.
The measured decisions still favor typed wiring on static paths and the handshake for concurrent teardown.

The consolidation adds:

- **Static-path concurrency regression coverage:** four threads share B1/B2 wiring and B3 `Sink`, with exact
  delivery/checksum checks and independent bool-cancellation decisions. Bindings are immutable and receivers
  provide synchronization. These tests run in the existing sanitizer jobs; `DynamicPort` remains single-threaded.
- **A remaining profiler failure fix:** Callgrind could exit nonzero after producing all phase dumps, and the
  evidence tool still accepted the measurement. It now checks the process status. The failing-before/fixed-after
  regression fixture preserves complete dumps to exercise this precise failure. Tests also guard empty selections,
  failed reference builds and a process that fails after printing its checksum.
- **Reader guidance:** the root README links the policy scores, static-wiring scores and decision record and
  explicitly distinguishes the experimental prototypes from the public header.

The earlier local checkout validated Release tests and the broker/quiescence/sandbox suites under TSan and
ASan/UBSan (local LeakSanitizer needed disabling because `/proc` access was unavailable). Workspace maintenance
removed that checkout before its push. The patch was restored from the review record and is revalidated before
publication; CI keeps its full, unmodified sanitizer configuration. No dispatch implementation or measured score
is changed by this consolidation patch.

### Remaining work, explicitly outside this design-study merge

- Port and measure deferred disconnect on the handshake; measure the empty-table optimization before promotion.
- Phase 2: integrate the selected structure into the per-message broker/public API, with migration guidance.
- Explain/measure K22–K24 compiler costs rather than assuming their root causes.
- Add MSVC/RISC-V evidence, ISR/deferred-dispatch designs and target RTOS lock measurements.
- Keep the documented DynamicPort mutation/lifetime restrictions visible; it is not a concurrent registry.

These are recorded gaps, not claims of completed or validated capabilities. The public header is unchanged.

Restored-checkout validation: GCC Release CTest **297/297 passed** after restoring missing execute bits on
seven generated ELF files; the broker, quiescence and sandbox suites also pass the local TSan run. The generated
file-mode repair is local environment maintenance, not a source change. Remote CI must pass on the published
commit before merging the stack.

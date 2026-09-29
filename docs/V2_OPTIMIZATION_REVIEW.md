# v2 optimization and migration review

Reviewed 2026-09-29 against `abf4c926` (v2 after PR #14). This review adds executable recipes and
mixed-path measurement sources. It does not claim a new runtime speedup or that every policy combination
has been benchmarked. The public API remains C++17; adopting a newer standard alone is not an optimization.

## Findings and decisions

| Priority | Finding | Action |
|---|---|---|
| High | Examples did not demonstrate the main v2 static and mixed APIs. A user could migrate everything to the runtime broker unnecessarily. | Five checked [v2 recipes](../examples/README.md), built and run by CTest, cover fixed, runtime, mixed and transport composition. |
| High | The migration comparison measured `DynamicPort` alone, not the cost of fixed delivery plus dynamic observers. | `cmp_mixed.cpp`: `wire` + DynamicPort and Scoped BrokerPort (Direct/Snapshot), with 0/1/8 dynamic listeners, successful churn and full-table rejection. Separate report table prevents unequal-work comparisons with pure v1 dispatch. |
| Medium | The basic example announced destruction of a still-live LCD subscriber. | Corrected the lifetime narrative. |
| Medium | Prototype tests and reports dominate the development tree and default build. They are still dependencies of evidence tools. | Stage removal by dependency and proof, not by deleting every file containing `sub0x`; see [cleanup](V2_CLEANUP.md). |
| Medium | The legacy MSVC final-link evidence gap remains; throughput under contention is not characterized by uncontended lock benchmarks. | Keep explicit release/evidence follow-ups. A passing Windows correctness build is not a performance result. |

## Where optimization is worthwhile

- **Fixed topology:** `StaticWiring` or `wire` keeps receiver types known. Stored Phase 3 evidence has 270/270
  static rows identical to their prototypes; the public-vs-handwritten gate is the source of collapse claims.
  Use `Publisher` for ergonomics: stored mixin rows match manual output storage (48/48). Avoid `Sink` unless
  a non-template boundary needs it; it intentionally introduces a type-erased call, though optimization may remove it.
- **Runtime topology:** keep Direct/NoContext/NoFilter when the contract allows it. Do not apply Full policy
  globally to emulate v1 if only one message type needs filtering or cancellation. Stable ordered removal is O(N);
  swap-with-last would break the documented order. Measure churn separately from steady-state publish.
- **Mixed topology:** isolate runtime observers behind one port so the controller stays statically bound.
  DynamicPort has no ownership, duplicate/null validation, filter, locking or safe callback mutation.
  BrokerPort is the appropriate choice for those broker policies; a cheaper slot table is not equivalent to it.
- **Snapshot:** stack storage scales with configured capacity, even when few listeners are active; nested publication
  multiplies that stack requirement. Tune per-type Capacity to the actual bound. Add stack high-water measurements
  before using large capacities or deep nesting on a small target. Current text/RAM figures do not prove stack safety.
- **Locked broker:** active snapshots, callback tracking and sequentially consistent atomics support quiescent
  disconnect. Do not weaken memory ordering or serialize callbacks under the table lock merely to match v1 timing.
  A custom lock must satisfy the lock contract; a spin lock's uncontended result does not establish interrupt safety
  or superiority under contention. Benchmark 1/2/4 publishers plus teardown latency before changing this path.
- **Transport:** Forward deliberately leaves acceptance handling to the transport; Route records it at extra cost.
  Neither means remote delivery. Measure serialization and queueing separately from dispatch and route acceptance.

Stored measurements: [v1/v2 comparison](perf/compare-v1-v2-2026-09.md),
[public final-link evidence](perf/collapse/phase3-public-api-2026-09.md).
Numbers are compiler/flags-specific. Debug checks, optional policy costs and lifetime guarantees must accompany comparisons.

## Coverage ledger

This is a map of supported use-case families, not a claim of an exhaustive Cartesian product of policies.
`tests/config` and `tests/wiring` test public headers; prototype-only coverage cannot replace them.

| Use case | Runnable example | Correctness coverage | Performance coverage |
|---|---|---|---|
| Runtime 0/1/many listeners and RAII | basic_pubsub; dynamic_lifetime | test_pubsub, test_defaults | cmp_sub0pub, bench_core; collapse dynamic_subscriptions |
| Static addresses and runtime addresses, fixed types | static_paths | wiring/test_wiring | cmp_static; collapse zero_receivers, one_receiver, multi_receivers |
| Publisher mixin; type-erased output | static_paths | wiring tests; collapse behavior checks | collapse `sub0_b1_mixin`, `sub0_b3_sink` |
| Multiple message types and nested dispatch | static_paths; multi_type | wiring/test_wiring; config/test_axes | collapse multi_types; bench_core nested |
| Filtering and cancellation | filtering; cancellation | test_cancel; config/test_axes; wiring/test_wiring | cmp_config/cmp_static/cmp_sub0pub; bench_core; collapse cancellation |
| Capacity failure and retry | dynamic_lifetime; mixed_paths | test_capacity_bounded; wiring/test_wiring | cmp_mixed full-table rejection (new) |
| Subscription churn outside dispatch | basic_pubsub; mixed_paths | public broker/wiring tests | cmp_mixed (new); collapse static_dynamic_bridge_churn |
| Removal/destruction during dispatch | dynamic_lifetime; mixed_paths (BrokerPort) | config/test_axes and ASan; wiring/test_wiring | Snapshot steady-state measured; callback-mutation cost not separately budgeted |
| Scoped isolation and close | dynamic_lifetime | config/test_endpoints | collapse two_domains; cmp_mixed scoped steady-state (new); close latency not separately budgeted |
| Locked registration and teardown | thread_safe_lifetime | config/test_axes, test_endpoints; TSan | cmp_config/bench_core uncontended; contention/teardown-tail benchmark still needed |
| Static + DynamicPort (empty/populated/churn) | mixed_paths | wiring/test_wiring; collapse checks | cmp_mixed (new); collapse bridge/empty/churn |
| Static + BrokerPort (Direct/Snapshot) | mixed_paths | wiring/test_wiring; collapse checks | cmp_mixed (new); collapse bridge/empty/churn (Direct) |
| Transport split horizon, multiple routes, rejection | transport_paths | config/test_endpoints; wiring/test_wiring | collapse transport_endpoint; rejection-specific cost still needed |
| Binary IPC and layout | ipc_pipe; layout_check | test_serialization; test_fingerprint | bench_ipc, footprint |
| Cross-translation-unit wiring | multi_type; public multi-TU tests | config/test_multi_tu_*; collapse cross_file | collapse with/without LTO |
| Cross-DLL state sharing and lifetime | Preserved [cross_module](../examples/cross_module/README.md), currently disabled | Public-API shared-state and unload tests still needed; separate-TU tests are insufficient | Shared-library dispatch measurement still needed |
| Arbitrary ISR invocation | No supported recipe | No portable guarantee | Not a release performance claim |

## Reproduce

```sh
cmake --preset default && cmake --build --preset default
ctest --preset default
python3 tests/compare/compare_versions.py > comparison.md
python3 tests/collapse/collapse_evidence.py --budgets tests/collapse/budgets.json --json evidence.json > evidence.md
```

The comparison requires Valgrind and its headers; use GCC and Clang for host instruction counts.
CI publishes the refreshed migration comparison with the final-link evidence. The mixed rows include one
fixed callback in every publication and identify dynamic occupancy explicitly. They are measurements, not
new hard budgets; the existing 510-row public final-link budget gate remains in force.

# Examples

Each source starts with a short guide: **Use when**, **Demonstrates**, **Story**, **Keep in mind**, and **Run**.
Choose a use case below, then read the source on its own.

```sh
cmake --preset default
cmake --build --preset default
ctest --preset default -R Sub0Pub_Example_
```

| Need | Source |
|---|---|
| The smallest runtime publisher and subscriber | [minimal_sub0pub](minimal_sub0pub/main.cpp) |
| Receivers join and leave through object lifetime | [basic_pubsub](basic_pubsub/main.cpp) |
| One publisher sends several message types | [multi_type](multi_type/main.cpp) |
| Different receivers accept different message values | [filtering](filtering/main.cpp) |
| A primary handler claims a command before a fallback | [cancellation](cancellation/main.cpp) |
| Wire known local receivers directly | [local_wiring.cpp](v2/local_wiring.cpp) |
| Hide the receiver-list type behind a `Sink` | [sink_output.cpp](v2/sink_output.cpp) |
| Encode static receiver addresses in the wiring type | [static_addresses.cpp](v2/static_addresses.cpp) |
| Stop delivery in fixed wiring with a bool result | [static_cancellation.cpp](v2/static_cancellation.cpp) |
| A bounded session with a one-shot recorder and a waiting recorder | [dynamic_lifetime.cpp](v2/dynamic_lifetime.cpp) |
| Two publishing threads share a receiver | [thread_safe_lifetime.cpp](v2/thread_safe_lifetime.cpp) |
| Explicitly add/remove a diagnostic probe beside a fixed controller | [dynamic_diagnostics.cpp](v2/dynamic_diagnostics.cpp) |
| Diagnostic probes disconnect during delivery using broker policy | [scoped_diagnostics.cpp](v2/scoped_diagnostics.cpp) |
| Forward over a link without immediately echoing incoming messages | [link_forwarding.cpp](v2/link_forwarding.cpp) |
| Forward with transport and receiver addresses fixed in the wiring type | [static_link_forwarding.cpp](v2/static_link_forwarding.cpp) |
| Report transport rejection while preserving local delivery | [route_reports.cpp](v2/route_reports.cpp) |
| Serialize typed messages and replay them from a byte buffer | [ipc_pipe](ipc_pipe/main.cpp) |
| Inspect message layouts before exchanging raw bytes | [layout_check](layout_check/main.cpp) |
| Investigate a shared-library / DLL boundary (currently disabled) | [cross_module](cross_module/main.cpp), [status](cross_module/README.md) |

The eleven focused v2 recipes return a failure code when their checks fail, including with `NDEBUG`.
The minimal example also checks its result; the other enabled introductory examples print their story.
These are teaching examples, not performance evidence. Cross-DLL support remains an explicit, retained gap.
See [migration](../MIGRATION.md), [coverage and optimization review](../docs/V2_OPTIMIZATION_REVIEW.md),
and [release cleanup gates](../docs/V2_CLEANUP.md).

Follow the mandatory [sample-header convention](../STYLE_GUIDE.md#examples-a-source-first-reading-guide).
Keep one independently useful pattern per sample; split alternatives a developer would select separately.
Use activity-based names and short comments for non-obvious contracts. Review with a source-only first reader,
so the header and code together explain who publishes, who receives, and how their lifetimes relate.

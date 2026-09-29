# Examples

Build and run the checked v2 recipes:

```sh
cmake --preset default
cmake --build --preset default
ctest --preset default -R Sub0Pub_Example_
```

| Need | Start here | Contract to keep |
|---|---|---|
| Fixed receivers; `wire`, `StaticWiring`, `Publisher`, `Sink`; multiple types and cancellation | [static_paths.cpp](v2/static_paths.cpp) | Receivers outlive wiring; wiring outlives `Sink`. Cancellation uses `publishCancelable`. |
| Runtime subscription; filter/cancel; capacity, scoped lifetime, self-removal, thread-safe registration | [dynamic_lifetime.cpp](v2/dynamic_lifetime.cpp) | Mutation during delivery needs Snapshot. Locked subscribers register last and disconnect first. Callbacks may run concurrently. |
| Fixed controller with optional runtime observers | [mixed_paths.cpp](v2/mixed_paths.cpp) | `DynamicPort` needs explicit removal and no mutation during delivery; `BrokerPort` supplies policy and scoped lifetime. |
| Transport ingress/egress and backpressure | [transport_paths.cpp](v2/transport_paths.cpp) | Split horizon prevents immediate echo, not arbitrary graph cycles. Acceptance is not remote delivery. |
| Traditional v1-style publisher/subscriber classes | [basic_pubsub](basic_pubsub/main.cpp), [multi_type](multi_type/main.cpp) | v2 adds `noexcept`; restore filter/cancel explicitly. |
| Focused filter and cancellation recipes | [filtering](filtering/main.cpp), [cancellation](cancellation/main.cpp) | Configure the message type consistently in every translation unit. |
| Binary IPC and layout checking | [ipc_pipe](ipc_pipe/main.cpp), [layout_check](layout_check/main.cpp) | Peers need matching byte order, layout and type identity. |
| Small embedded setup | [minimal_sub0pub](minimal_sub0pub/main.cpp) | Disable only features the application does not use. |

The four v2 recipes return a failure code when their delivery/lifetime checks fail, even with `NDEBUG`.
The older print-oriented examples remain migration aids, not performance evidence.
See [migration](../MIGRATION.md), [coverage and optimization review](../docs/V2_OPTIMIZATION_REVIEW.md),
and [release cleanup gates](../docs/V2_CLEANUP.md).

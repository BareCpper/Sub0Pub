# Performance Baseline (v2 @ `9e04143`)

This is the bar that later v2 changes are measured against, in particular broker customisation
([docs/design/BROKER_CUSTOMISATION.md](design/BROKER_CUSTOMISATION.md)). **The zero-cost rule:** a change must not
raise any `instr/op` or byte count below for users who don't opt in to it.

Full reports:
- [perf/baseline-2026-09-linux-gcc13-bench.md](perf/baseline-2026-09-linux-gcc13-bench.md): every scenario, instr/op and ns/op
- [perf/baseline-2026-09-footprint.md](perf/baseline-2026-09-footprint.md): code size, RAM, sizeof, link dependencies

## How it is measured

| Metric | Tool | Use |
|---|---|---|
| **instr/op** | `valgrind --tool=callgrind`, per-scenario dumps via client requests (`tests/bench/bench_harness.hpp`) | Deterministic for a given compiler and flags: **the regression bar** |
| ns/op | nanobench | Machine- and noise-dependent: ±10–20% run to run on shared CI/VMs |
| text/data/bss, symbol sizes | `size`, `nm -S` on `-Os -fno-exceptions -fno-rtti` objects (`tests/footprint/`) | Embedded cost per usage pattern |

Reproduce:
```bash
cmake --preset default && cmake --build --preset default
python3 tests/bench/run_baseline.py build/tests      # instr/op + ns/op for every policy and IPC
python3 tests/footprint/measure_footprint.py         # host + Cortex-M33 (arm-none-eabi-g++ if installed)
```

**Control conditions.** Every scenario is the worst case: publish entry points and lifetime operations
are out of line, and subscribers are defined out of line with more than one implementation. The optimiser
therefore cannot inline or devirtualise dispatch at the benchmark site, and each row measures the same
generic `Broker<Data>::publish()` code. The "collapse target" floor shows what fully devirtualised dispatch
would cost. That gap is the headroom for the follow-up on compile-time collapse.

Environment: GCC 13.3 `-O3 -DNDEBUG` (CMake Release), Intel Xeon @ 2.8 GHz VM. Cortex-M33 footprint uses
arm-none-eabi-g++ 13.2.1, `-mcpu=cortex-m33 -mthumb -Os`.

## Dispatch cost (instr/op)

| Scenario | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| publish, 0 subscribers | 38 | 39 | 38 | 123 |
| publish, 1 subscriber | 72 | 60 | 59 | 157 |
| publish, 2 subscribers | 108 | 81 | 80 | 194 |
| publish, 4 subscribers | 155 | 123 | 122 | 241 |
| publish, 8 subscribers | 247 | 207 | 206 | 333 |
| 8 subscribers, first cancels | 87 | 59 | 58 | 173 |
| re-entrant publish, depth 1 | 147 | n/a | n/a | 312 |
| create + destroy subscriber | 61 | 61 | 66 | 218 |
| unsubscribe first of 8 + resubscribe | 88 | 88 | 92 | 237 |
| `trySubscribe()`, table full | 12 | 12 | 14 | 90 |

Floors, no Sub0Pub (identical in every build):

| Floor | 1 receiver | 8 receivers |
|---|---:|---:|
| direct call, compiler may inline (**collapse target**) | 6 | 13 |
| virtual `receive()` loop | 11 | 89 |
| virtual `filter()` + `receive()` loop (Sub0Pub's semantics) | 25 | 118 |
| `std::function` loop | 30 | 100 |

Representative wall-clock: 1 subscriber about 5 ns, 8 subscribers about 22–26 ns. The `filter()` + `receive()`
virtual-call floor is about 2 ns and about 12 ns respectively.

### Where the instructions go (Direct unchecked, from the disassembly)
- **Fixed cost of about 35 instructions per publish**, even with 0 subscribers:
  - call plus six callee-saved register saves and restores;
  - saving, setting and restoring two `thread_local` values (`threadCurrent_`, `threadCanceled_`) that support `cancel()` and nested publish.
- **About 21 instructions per subscriber:**
  - two virtual calls: `filter()`, then `receive()`;
  - the subscriber pointer is reloaded before each call;
  - a `thread_local` load of the cancel flag every iteration.

  The hand-written floor is about 13 per receiver. A `filter()` that is never overridden still costs a
  virtual call for every subscriber on every publish.
- **Snapshot policy (the default):** about +12 fixed and about +5 per subscriber. It copies the table to
  the stack (a `memcpy` call on Cortex-M33) before dispatch.
- **ThreadSafe:** about +85 fixed, from an uncontended `std::mutex` lock and unlock around the snapshot copy.
  Subscribe and unsubscribe cost about 3.5× the unlocked path.
- **Collapse headroom:** 1 subscriber costs 60–72 instructions against a target of 6, and 8 cost 207–247
  against 13. Most of the gap is the generic, out-of-line, virtual dispatch loop, not the work itself.

## IPC end-to-end (instr/op, default policy)

| Payload (framed bytes) | serialize: publish → stream | deserialize: stream → subscriber | floor: memcpy frame |
|---|---:|---:|---:|
| 4 B (17 B) | 133 | 472 | 23 |
| 64 B (77 B) | 139 | 469 | 34 |
| 256 B (269 B) | 155 | 487 | 65 |

- Cost is almost all **per message**, not per byte. Serializing makes four virtual `OStream::write`
  calls (prefix, header, payload, postfix).
- Deserializing costs about 470 instructions regardless of size:
  - a four-state machine, with one virtual `IStream::read` per state;
  - a type lookup in the `BufferRegister` table;
  - a copy into the `ForwardPublish` buffer, then a publish.
- Wall-clock: about 12–17 ns to serialize and about 40 ns to deserialize, against about 3–5 ns for the memcpy.
- **Finding:** `StreamDeserializer::update()` returns `true` after a frame *prefix* is read, not when a
  message is published, which contradicts its doc comment. The benchmark detects completion by counting
  deliveries instead.

## Footprint

Cortex-M33 (`-Os`, bytes; the full report also covers the host):

| Usage | text | bss | Snapshot → Direct |
|---|---:|---:|---|
| 1 type: 1 publisher, 1 subscriber, 1 publish site | 566 | 77 | text 566 → 526 |
| + 1 subscriber of the same type | +44 | | |
| + 1 publish call site of the same type | +24 | | |
| + 1 Data type (publisher, subscriber, site) | +554 | | +514 |
| 1 type forwarded to `StreamSerializer` | 704 | 65 | |

| Per-type item (Cortex-M33) | bytes |
|---|---:|
| `Broker<T>::publish()` | 152 (Snapshot), 112 (Direct) |
| `Broker<T>::unsubscribe()` | 156 |
| `Broker<T>::state_` (8-entry table + count) | 36 RAM |
| `threadCurrent_` + `threadCanceled_` | 5 TLS |
| `vtable for Subscribe<T>` | 24 |
| `sizeof(Subscribe<T>)`, `sizeof(Publish<T>)` | 8 each (vptr + per-instance broker flag) |

Link-time dependencies of the default build on Cortex-M33: `__aeabi_read_tp` (**thread-local storage
is required**), `operator delete` (from virtual destructors), `__cxa_pure_virtual`, `__aeabi_atexit`,
`memcpy` (snapshot), `memmove` (unsubscribe). With the reentrancy check enabled, `abort` is also needed.

### Embedded findings
1. **`SUB0PUB_THREAD_SAFE` does not compile on arm-none-eabi.** There is no `std::mutex`
   (`sub0pub.hpp:620`), so bare-metal and RTOS targets have no supported locking option today.
2. **TLS is mandatory.** Every build needs thread-local storage (Zephyr: `CONFIG_THREAD_LOCAL_STORAGE`),
   even single-threaded ones, because of the cancel and nested-publish context.
3. **`Publish<T>` has a virtual destructor whose body does nothing**, since publisher unsubscribe is a
   no-op. That costs a vtable pointer per publisher and pulls in `operator delete`.
4. **`filter()` is paid for by everyone.** It is a second virtual call per subscriber per publish, even
   when no subscriber overrides it.
5. **Capacity is global:** one `SUB0PUB_MAX_SUBSCRIPTIONS` sizes every type's table. On Cortex-M33 each
   type costs 4 bytes of RAM per slot whether it is used or not.

## Not yet measured
- Contended locking (several threads publishing the same type), and ISR-context publish.
- Cross-core or real-transport IPC (nRF54 IPC service), and receive-to-dispatch latency on target hardware.
- MSVC and Clang instruction counts (the numbers here are GCC). Windows timings are in `README.md`.
- RISC-V: the Ubuntu `riscv64-unknown-elf` toolchain ships without libstdc++, so this needs a full
  toolchain (for example Zephyr SDK) for the nRF54 VPR cores.

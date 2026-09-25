# Spike face-offs: decision record (issues #9, #5, #2)

Six spikes each put competing designs for one open question side by side, and measured them with the
collapse evidence loop ([COLLAPSE_EVIDENCE.md](../COLLAPSE_EVIDENCE.md): callgrind instructions, final-link
ELF on gcc-O2, clang-O2 and Cortex-M33 `arm-none-eabi-g++ -Os`) or the benchmark harness
([PERFORMANCE_BASELINE.md](../../PERFORMANCE_BASELINE.md)). Every claim below was re-run on integration;
where the re-run changed a spike's conclusion, the spike's own document says so and this record uses the
corrected result.

Judging rule, from the Sub0 design intent (correctness without cost): an alternative must first be correct,
then cost nothing over hand-written code where the user did not ask for the feature. Anything that costs
more is recorded as a known issue with its measured price
([BROKER_CUSTOMISATION.md section 8](../BROKER_CUSTOMISATION.md)).

## Decisions

| Question | Decision | Evidence (deltas against hand-written) | Runner-up / rejected | Spike |
|---|---|---|---|---|
| How does a hot-path publisher name its output? (#9) | **Static topology: a plain publisher naming the `StaticWiring` alias.** Dynamic topology: the CRTP mixin `sub0x::Publisher<Derived, Out>`. A TU or library boundary: `Sink<T>` | Direct alias is `=` on every build. The CRTP mixin costs the same as a hand-spelled `template<class Out>`: gcc +9 instr and +40 B RAM for the stored `Out&`, clang `=` | Call-site argument (no stored reference). Rejected: the CTAD factory (clang +5/+8 instr on top) | [publisher_ergonomics.md](publisher_ergonomics.md) |
| Can a static-path receiver stop the rest of a publication? (#9) | **`bool receive()`**, where `false` stops. Detected at compile time and short-circuited by a fold | `=` on every build and both forms; no shared or thread-local state | `receive(const T&, Delivery&)` token (`=`). Rejected: thread-local `cancel()` (gcc +3.0 instr; Cortex-M33 +14 path instr, +257 B RAM, TLS dependency) and filter-based control (+1.0 to +9.3 instr) | [static_cancellation.md](static_cancellation.md) |
| How do static wiring and runtime subscribers meet? (#9) | **`DynamicPort<T, N>`**: an intrusive slot array bound as one more receiver of the static wiring | gcc +1 publish instr, clang `=`, Cortex-M33 path `=`. Empty port: gcc +3, clang +11, Cortex-M33 +6 | `BrokerPort<T>` over the #8 registry, for when the dynamic side needs policy (+36 to +41). Rejected: routing static receivers through the registry (+78 to +86, and +35 to +68 even when empty) | [static_dynamic_bridge.md](static_dynamic_bridge.md) |
| What makes `disconnect()` safe against concurrent delivery? (#5) | **Mechanism 1, the handshake the #8 prototype already uses**, pending round 2 below | Correct in every probe. 8 subscribers: 256 instr/op, which is below the old `Locked` baseline's 347 | Hazard pointer and epoch measured cheaper (165 and 118), but both **fail** the lifetime probes: self-disconnect deadlocks; nested publish and a 9th thread are use-after-free windows | [quiescence.md](quiescence.md) sections 9-10 |
| What shrinks the embedded image? (#2) | **A protected, non-virtual destructor** on `Subscribe`/`Publish` (the v2 base), a documented `-fno-rtti` expectation, and TLS as a configuration axis | One-receiver Cortex-M33 image: 5452/88/837 today → 4632/8/813 (text/data/bss) with the destructor change alone; it also drops `operator delete`/`malloc` from the link. RTTI on: +1.3 KB | `filter()` as a capability (+24 to 28 B; already how v2 works) | [embedded_size.md](embedded_size.md) |
| Should the project move past C++17? | **Stay C++17-required.** C++20 concepts and `consteval` type hashing are optional, feature-gated extras. C++23 deducing this is never required | Concepts: codegen byte-identical on all three compilers. Deducing this: GCC 13 and `arm-none-eabi-g++` 13 reject it (it arrives in GCC 14); where it builds it costs the same as the C++17 mixin | `std::expected` returns (cancellation Alt 1c, bridge bind result) measure identical to their C++17 forms but are missing with clang + libstdc++ 13 | [cxx23_upgrade.md](cxx23_upgrade.md) |

## The v2 shape these add up to

- **Hot paths use pattern B.** Receivers are plain classes with non-virtual `receive()`, bound at the
  composition point. `StaticWiring` gives hand-written code. Publishers name the wiring, use the CRTP mixin,
  or use `Sink<T>` at a real boundary, in that order of cost. Receivers stop a publication by returning
  `false`. None of this needs a registry, per-message configuration, TLS or C++ newer than 17.
- **Dynamic subscribers live behind an explicit boundary.** A `DynamicPort` in the static wiring covers the
  common case. `BrokerPort` routes to the #8 runtime registry when that side needs capacity, filtering,
  locking, domains or the concurrent disconnect contract.
- **The #8 runtime registry keeps the handshake teardown.** Its cost stays known issue K3 until a cheaper
  mechanism passes the same lifetime probes. `disconnectLater()` (K4) and a CRTP activate-after-construct
  helper (K5) layer on top of it without changing the choice.
- **The v2 base types have no virtual destructor.** That is the largest single embedded size lever
  measured, and pattern B needs no virtual base at all.

## What integration changed

Spike reports were checked, not trusted. These corrections were made on integration:

- **Cancellation.** The hand-written reference stored the stop decision in a member, which set the bar
  3 instructions and 8 B too low, so the recommended alternatives appeared to beat hand-written code.
  Against the tightened reference they are exactly `=`, and the thread-local alternative gained a gcc cost
  the spike had missed.
- **Quiescence.** The recommended mechanism and the runner-up both failed three lifetime probes written
  on integration (`tests/design/quiescence/probe_lifetime.cpp`, now a ctest target). The recommendation was
  reversed, and a second round fixes them and re-measures with the fixes' cost included.
- **C++23 harness support.** Two spikes had added separate ways to build a variant at a newer standard.
  They are now one mechanism: source markers `// SUB0X_STD: c++NN` and `// SUB0X_REQUIRES: <feature>`,
  read by CMake and `collapse_evidence.py`, with compiler-agnostic feature probes.
- **Build and test hygiene.** A duplicated test case was folded back. A C++20 test target that was
  registered with ctest but excluded from the default build was fixed. The cross-thread teardown test in
  `test_endpoints.cpp` now waits for the first publication, so its race is always exercised.

## Still open

- Quiescence round 2: fixed hazard pointer and epoch mechanisms, re-measured ([quiescence.md](quiescence.md)).
- MSVC evidence (`dumpbin`) and the equal-work runtime-address reference for B1 (COLLAPSE_EVIDENCE.md plan).
- Phase 2 of #9: #8's broker specialisation selecting the static structure per message type or domain.
- Issue #11 (`SUB0PUB_TYPEIDNAME` does not compile), found by the embedded spike.

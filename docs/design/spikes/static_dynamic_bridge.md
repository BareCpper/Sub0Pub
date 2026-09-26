# Spike: static <-> dynamic bridge (issue #9 open item)

> **Fairness review (2026-09): see the "Fairness review" section at the end; it supersedes the evidence tables.**

**Question:** applications mix a zero-cost static wiring (pattern B, `tests/collapse/sandbox/sub0x_static.hpp`)
with a runtime registry at a genuinely dynamic boundary (plugins, diagnostics, late subscribers). How do the
two join, so the static part keeps its measured zero cost and the dynamic part pays only its own cost?

**Method:** three alternatives measured against a hand-written equal-work reference, using the collapse
evidence loop (`docs/design/COLLAPSE_EVIDENCE.md`). New cases: `tests/collapse/cases/static_dynamic_bridge`
(sensor -> controller, logger, one dynamic probe subscribed at setup) and
`tests/collapse/cases/static_dynamic_bridge_empty` (same shape, dynamic side never populated). New prototype
header: `tests/collapse/sandbox/sub0x_bridge.hpp`. All 14 new ctest cases pass (behaviour identical to
handwritten, `ctest --preset default -R Collapse_static_dynamic_bridge`) -- 12 for the three C++17 alternatives
plus 2 for a C++23 variant of alternative B (see "C++23", below).

```
python3 tests/collapse/collapse_evidence.py --case static_dynamic_bridge
python3 tests/collapse/collapse_evidence.py --case static_dynamic_bridge_empty
```

(The C++23 variant opts in with a first-line `// SUB0X_STD: c++23` marker, read by both CMake and
`collapse_evidence.py` — see [cxx23_upgrade.md](cxx23_upgrade.md); every other variant stays on C++17.)

## Scenario

A sensor publishes `Sample` to two static receivers (`Controller` gain 3, `Logger`) and to a runtime registry
holding one dynamic subscriber (`Probe`, `COLLAPSE_WORK(s.value + 11U)`) subscribed during setup. Required
order: controller, logger, then dynamic subscribers. The empty variant drops the dynamic subscriber but keeps
the bridge element bound, to isolate what an *available but unused* dynamic side costs.

Handwritten references: direct calls to controller and logger, plus (main case only) a minimal hand-written
dynamic registry (fixed array + virtual `receive`) for the probe; the empty reference has no registry at all
-- the ideal code for "no dynamic subscriber, ever" skips it entirely.

## Alternatives

### A -- `BrokerPort<T>`: bridge element forwards to the #8 runtime registry

```cpp
struct Sample { uint32_t value; using sub0_config = sub0x::config<sub0x::Scoped>; };
collapse::Slot<sub0x::Domain<Sample>> domain;
collapse::Slot<sub0x::BrokerPort<Sample>> port;      // : sub0x::Publish<Sample>
collapse::Slot<Probe> probe;                          // : sub0x::Subscribe<Sample>
using Bus = sub0x::StaticWiring<&controller, &logger, &port>;   // port is bound last

port.emplace(domain.get());
probe.emplace(domain.get());       // #8 contract: constructs against the Domain, disconnects in its dtor
```

`BrokerPort` is a two-line adapter (`tests/collapse/sandbox/sub0x_bridge.hpp`): it derives from
`sub0x::Publish<T>` and exposes `receive(const T&)` that calls the (protected) `publish()`. Binding it as one
entry of `StaticWiring`/`Wiring` makes it visible to `detail::accepts`, so it is called in bound position like
any other receiver. Everything the #8 prototype already offers (filter, capacity, `Domain` scoping, the
disconnect contract, thread-safety policy) applies to the dynamic side only.

### B -- `DynamicPort<T, N>`: bridge element owns a minimal intrusive slot array

```cpp
collapse::Slot<sub0x::DynamicPort<Sample>> port;      // fixed array, N = 8 default, no broker
collapse::Slot<Probe> probe;                          // : sub0x::DynamicPort<Sample>::Receiver
using Bus = sub0x::StaticWiring<&controller, &logger, &port>;

port.emplace();
probe.emplace();
port->add(&probe.get());          // caller-managed; port->remove() before the probe is destroyed
```

No dependency on `sub0x_broker.hpp` at all: a fixed `Receiver* entries[N]`, `add`/`remove`/`receive`, nothing
else -- no filter, no publish context, no locking, no capacity beyond `N`. The least a dynamic side can cost.

### C -- `StaticAdapter<Bus, T>`: invert the direction (for comparison)

```cpp
using Bus = sub0x::StaticWiring<&controller, &logger>;      // no dynamic entry in the static wiring
collapse::Slot<sub0x::StaticAdapter<Bus, Sample>> adapter;  // : sub0x::Subscribe<Sample>, registered FIRST
collapse::Slot<Probe> probe;                                 // : sub0x::Subscribe<Sample>, registered second
struct Sensor : sub0x::Publish<Sample> { void send(uint32_t v) { sub0x::publish(*this, Sample{v}); } };

adapter.emplace(domain.get());
probe.emplace(domain.get());
```

The publisher publishes straight into the #8 registry; the static wiring is itself registered as one
subscriber (`StaticAdapter::receive` calls `Bus::publish`), ahead of the dynamic `Probe`, so ordering is
preserved. This is the reverse-direction shape from the brief ("the static wiring registers itself as ONE
subscriber ... dynamic -> static direction"), applied here to answer the *same* forward scenario by routing
everything -- static and dynamic alike -- through one dispatch. It is included specifically to measure that
cost, not as a candidate to adopt.

**Both directions, coverage:** A and B answer "static publisher -> dynamic subscribers" (the required
scenario) directly, at the static side's normal zero/near-zero cost. C demonstrates "dynamic publisher ->
static receivers" and shows what folding the two into one dispatch costs the static side, which A and B never
pay. A bidirectional application would use A or B for its hot static-published path and, separately, a plain
`Subscribe<T>`-derived adapter like `StaticAdapter` only at whatever boundary a genuinely dynamic publisher
needs to reach static receivers -- not as a replacement for the static wiring's own dispatch.

## Evidence: `static_dynamic_bridge` (dynamic side populated)

Deltas are against the hand-written reference, same build and form. "path" = static instructions of
`collapse_publish` reachable by direct calls; "calls" = direct/indirect call counts on that path.

### gcc-O2, observable work (publish instr is per-publication average)

| variant | publish instr | setup instr | teardown instr | data+bss | calls (dir/ind) | added deps |
|---|---:|---:|---:|---:|---:|---|
| handwritten | 36.0 | 27 | 16 | 760 B | 0/0 | - |
| **A** `sub0x_bridge_broker` | 72.0 (+36.0) | 51 (+24) | 92 (+76) | 968 B (+208) | 2/0 | operator delete |
| **B** `sub0x_bridge_slots` | 37.0 (+1.0) | 31 (+4) | 32 (+16) | 800 B (+40) | 0/0 | - |
| **C** `sub0x_bridge_inverted` | 122.0 (+86.0) | 75 (+48) | 148 (+132) | 1072 B (+312) | 2/1 | operator delete |

### clang-O2, observable work

| variant | publish instr | setup instr | teardown instr | data+bss | calls (dir/ind) | added deps |
|---|---:|---:|---:|---:|---:|---|
| handwritten | 58.0 | 25 | 14 | 776 B | 0/1 | - |
| **A** | 99.0 (+41.0) | 40 (+15) | 73 (+59) | 976 B (+200) | 1/2 | operator delete |
| **B** | 58.0 (+0.0) | 27 (+2) | 30 (+16) | 808 B (+32) | 0/1 | - |
| **C** | 136.0 (+78.0) | 57 (+32) | 123 (+109) | 1072 B (+296) | 1/2 | operator delete |

### cm33-gcc-Os (final-ELF only: publish path instr, text, RAM)

| variant | path instr | text | data+bss | calls (dir/ind) | added deps |
|---|---:|---:|---:|---:|---|
| handwritten | 45 | 1252 B | 536 B | 1/0 | - |
| **A** | 57 (+12) | 3428 B (+2176) | 928 B (+392) | 1/1 | TLS, operator delete |
| **B** | 45 (+0) | 1568 B (+316) | 552 B (+16) | 1/0 | - |
| **C** | 54 (+9) | 3632 B (+2380) | 944 B (+408) | 2/2 | TLS, operator delete |

**B is within noise of handwritten everywhere** (publish +0 to +1 instr, no extra indirect call on gcc/cm33,
one already-present indirect call carried over from handwritten on clang). **A** costs one bound virtual call
worth of dispatch (its indirect call is devirtualized to direct on gcc since `Probe` is `final` and the whole
program is visible, but stays indirect on clang and cm33) plus the #8 registry's usual setup/teardown/RAM
(`Domain`, `Subscribe` vtable/typeinfo, disconnect machinery) -- consistent with the dynamic-subscriptions
regression limits already recorded in `COLLAPSE_EVIDENCE.md`. **C is markedly worse on every axis**: it pays
the full registry dispatch (virtual call, `Domain`, `Subscribe` destructor/disconnect) not just for the probe
but *to reach the static receivers at all* -- its `StaticAdapter` typeinfo/vtable/destructor is pure overhead
that A and B never incur.

## Cost when the dynamic side is empty (`static_dynamic_bridge_empty`)

| build, alt | publish instr delta | setup delta | teardown delta | data+bss delta | indirect calls | added deps |
|---|---:|---:|---:|---:|---:|---|
| gcc, **A** | +24.0 | +9 | +20 | +136 B | 0 | pure virtual |
| gcc, **B** | +3.0 | +6 | +0 | +96 B | 0 | pure virtual |
| gcc, **C** | +35.0 | +33 | +76 | +344 B | 0 | operator delete |
| clang, **A** | +35.0 | +9 | +14 | +120 B | 2 | - |
| clang, **B** | +11.0 | +6 | +0 | +72 B | 1 | - |
| clang, **C** | +68.0 | +23 | +59 | +312 B | 2 | operator delete |
| cm33, **A** (path) | +36 | - | - | +408 B | 1 | TLS |
| cm33, **B** (path) | +6 | - | - | +36 B | 0 | - |
| cm33, **C** (path) | +36 | - | - | +416 B | 1 | TLS, operator delete |

**B is the only alternative that approaches zero when unused**: a few instructions of loop-guard overhead
(`count_` is read at runtime, not compile time -- `collapse_setup`/`collapse_publish` are separate,
`noinline` translation-boundary functions by the harness's own design, so the compiler cannot fold across
them even though `count_` never leaves 0 in this program). No allocation, no vtable, no destructor chain; the
only "added dep" is `__cxa_pure_virtual`-style support pulled in by `Receiver`'s pure-virtual `receive`, which
a concrete no-op default (rather than `= 0`) would remove entirely if that mattered for a given target.
**A never gets near zero**: constructing `Domain`/`BrokerPort` still builds and tears down the registry's
table and (on cm33) pulls in TLS -- the dynamic side's *capability*, not its occupancy, is what costs. **C
cannot get near zero either, and that is its defining flaw**: because the static receivers are now reached
*through* the registry, the `StaticAdapter` must stay registered regardless of whether any dynamic subscriber
exists, so the full per-publish dispatch and per-teardown disconnect cost is paid unconditionally -- worse
than A's on every row above, for a case with *no* dynamic subscriber at all.

## Semantics notes

- **Ordering.** All three alternatives preserve the required controller / logger / dynamic order: A and B by
  binding the bridge element last in the static wiring's argument list (bound order = delivery order, same
  rule as every other pattern-B case); C by registering `StaticAdapter` before `Probe` in the registry (table
  order = delivery order there too). Ordering is a binding-time choice in all three, not a language guarantee
  -- getting it wrong is a silent behaviour bug, not a compile error, in any of them.
- **Split horizon / echo.** Not exercised by this scenario (egress only, no dynamic-side publish back to the
  static side), so this is a design note rather than a measurement. A inherits the #8 registry's existing
  `origin`/`Frame` mechanism (`tests/collapse/cases/transport_endpoint`, `Route`), so a `Subscribe<T>`-derived
  dynamic endpoint that also publishes gets split-horizon protection for free if it publishes through
  `sub0x::publish`. **B has no split-horizon support at all** -- `DynamicPort` is a bare broadcast list with
  no origin tracking; a dynamic receiver that publishes back into the same port would echo. This is a real
  gap for B if it is ever used bidirectionally (e.g. a dynamic subscriber that is also a `Wiring::publishFrom`
  participant); acceptable for the egress-only, plugin/diagnostics-observer shape this spike targets, not for
  a two-way dynamic peer. C inherits the registry's split horizon the same way A does, but since the static
  side is reached *through* the registry, a message forwarded by `Bus::publish` from inside `StaticAdapter`
  cannot itself be filtered from re-entering the registry without extra bookkeeping this prototype does not
  add.
- **Lifetime / teardown, and the #8 disconnect contract.** A and C both bind through `sub0x::Domain<T>` /
  `sub0x::Subscribe<T>`, so they inherit the #8 contract as-is: `Subscribe` disconnects in its own destructor,
  `Domain`'s destructor asserts no bound handles remain, and the case's teardown order (destroy
  `port`/`adapter`/`probe` before `domain`) is exactly the ordering that contract already requires elsewhere
  (`tests/design/broker_config/sub0x_broker.hpp`'s `Domain::~Domain`). Nothing new was needed there. **B has
  no destructor-driven contract at all** -- `add`/`remove` are plain calls the application must sequence
  itself (the case's teardown calls `port->remove(&probe.get())` before destroying `probe`); skipping that
  step leaves a dangling pointer in the array with no detection. A thin RAII handle around `add`/`remove`
  would close this gap at a small, measurable cost (not attempted here) if B is taken further.

## C++23

The maintainer asked whether a move to C++23 would materially simplify the bridge. Three candidate features
were evaluated against the actual code in this spike, on the stated toolchains (g++ 13, clang++ 18):

- **`std::expected` for `DynamicPort::add()`'s result.** Today `add()` silently drops the receiver if the
  fixed array is full -- the only reporting gap this spike's alternatives have (see "Semantics notes" above).
  This is a genuine, if modest, fit: added `DynamicPort::tryAdd()` (returns `bool`, C++17, in
  `sub0x_bridge.hpp`, used unconditionally by `add()` too) and a call-site wrapper,
  `cases/static_dynamic_bridge/sub0x_bridge_slots_cpp23.cpp`, that reports
  `std::expected<void, BridgeError>` where the toolchain provides it. **Measured cost: exactly zero** -- every
  column (publish/setup/teardown instr, text, RAM, calls) is byte-for-byte identical to the C++17
  `sub0x_bridge_slots` baseline on gcc-O2, clang-O2 and cm33-gcc-Os, both forms. `std::expected<void, E>`'s
  discriminant is not observable here because the result is discarded (`(void)demo::addChecked(...)`,
  matching how a real bind-time check would use it: check once, keep going).
  **Toolchain gap found:** libstdc++ 13's `<expected>` gates on `__cplusplus > 202002L && __cpp_concepts >=
  202002L`; clang++ 18 in `-std=c++23` mode does not report a high enough `__cpp_concepts`, so `std::expected`
  is unavailable through that specific compiler/library pairing (arm-none-eabi-g++ 13, being real GCC, is
  unaffected). The variant file therefore feature-tests `__cpp_lib_expected` and falls back to a plain `bool`
  where it is absent, so it still builds -- and still measures identically -- on every named build; only GCC's
  and arm-none-eabi-GCC's numbers reflect `std::expected` actually being used.
- **Deducing this (explicit object parameter).** Considered for collapsing `BrokerPort`/`StaticAdapter`'s
  paired constructors (one for `Storage::Global`, one taking `Domain<T>&` for `Storage::Scoped`) into one.
  It does not fit: deducing this changes how the *implicit object parameter* of a member function is named and
  deduced (useful for merging const/non-const or value-category overloads, or CRTP without a base class); it
  has nothing to do with selecting between constructor *signatures*, which is what varies here. Not adopted;
  not applicable to this code. Also: **g++ 13 does not support it** (added in GCC 14) -- confirmed by a direct
  compile attempt -- so even if it had fit, it could not have been the measured baseline on this toolchain.
- **`static operator()`.** Confirmed to compile under both g++ 13 and clang++ 18 at `-std=c++23`, but every
  bridge element here (`BrokerPort`, `DynamicPort`, `StaticAdapter`) dispatches through per-instance state
  (a table, an array, a `Domain&`), which a `static` call operator cannot carry. Not applicable.
- **`if consteval`** has no bearing on this spike (nothing here branches on constant evaluation).

**Conclusion:** C++23 does not simplify the bridge's actual costs, which are dispatch (virtual calls, one
indirect call through `Sink`-style ports) and lifetime (the #8 registry's construction/destruction and
disconnect contract) -- none of which any evaluated C++23 feature changes. It does let alternative B report a
bind-time capacity failure through the vocabulary type applications will already use elsewhere, at measured
zero cost, with a feature-tested fallback where the toolchain lacks it. That is worth carrying into the real
API if/when the project moves to C++23, but it is an ergonomics improvement to B, not a reason to prefer a
different alternative, and not a blocker to adopting B under C++17 now.

## Recommendation

**B (`DynamicPort<T, N>`, the intrusive slot array bound as one entry of the static wiring) for the common
case**, with **A (`BrokerPort<T>` over the #8 registry) as the runner-up** for when the dynamic side actually
needs the #8 policy surface -- filtering, capacity limits, `Domain` scoping/closing, or the concurrent
disconnect contract. Both answer the forward direction (static publisher -> dynamic subscribers) cleanly and,
critically, both let the static receivers keep their already-proven zero cost: the bridge element is just
one more bound receiver, in one more position, exactly like `Sink<T>` or `Forward<Transport>` already are in
pattern B. Do not adopt C's inversion: routing the always-known static receivers through the registry's
virtual dispatch to share one call site with the dynamic ones costs the static side something on every build
and every form measured here, including -- worst of all -- when there is no dynamic subscriber to justify it.

Practical guidance for #8/v2: expose B as the default "add a dynamic boundary to a static wiring" facility
(smallest surface, smallest cost, no registry dependency to pull in at all if the application has none), and
document A as the escape hatch for when that boundary needs real policy. Neither needs to become part of
`StaticWiring`/`Wiring` itself -- both are ordinary bindable receivers, which is the reason binding order,
`detail::accepts`, and `publishFrom`'s split-horizon machinery already work for them unmodified.


## Fairness review (2026-09)

The first round's references did less work than the bridges, and A and C ran the #8 registry in its default
configuration:
- **Populated case:** the hand-written registry had 4 slots and no removal, while `DynamicPort` has 8 slots and
  the variants unsubscribe at teardown. The reference now has 8 slots, the same order-preserving `remove()`,
  and removes the probe at teardown.
- **Empty case:** the reference had no registry at all, which prices *having* a dynamic side, not the bridge.
  `handwritten` keeps that meaning; the bridges are now judged against `handwritten_registry`, a hand-written
  program that can accept dynamic subscribers but has none.
- **Registry configuration:** A and C now use `config<Scoped, Direct, NoContext, NoFilter>`, the features the
  hand-written registry has, instead of the default snapshot dispatch, cancel context and filter.

| | gcc-O2 publish | clang-O2 publish | cm33 path | setup / teardown (gcc) | text / RAM (gcc) |
|---|---|---|---|---|---|
| B `DynamicPort` populated | = | = | = | = | +48 B / = |
| B `DynamicPort` empty | = | = | = | = | = |
| A `BrokerPort` populated | = | -1.0 | +3 | +20 / +54 | +1461 B / +144 B |
| A `BrokerPort` empty | +1.0 | = | = | +3 / +19 | +465 B / +24 B |
| C inverted populated | +37.0 | +10.0 | -21 (behind an indirect call) | +44 / +105 | +2174 B / +240 B |
| C inverted empty | +7.0 | +15.0 | -7 | +27 / +70 | +2202 B / +224 B |

The price of accepting dynamic subscribers at all (`handwritten_registry` against `handwritten`, empty case):
publish gcc +3, clang +11, Cortex-M33 path +6, RAM +72 to +96 B. That is what the first round reported as
`DynamicPort`'s cost (K12, withdrawn): hand-written code pays it too.

**Revised recommendation.** Unchanged, better founded: **B** is exactly the hand-written registry, populated or
empty. **A** is now `=` on publish with the lean registry, and its costs are setup/teardown, about 1.5 KB of image
and the #8 dependencies, the price of the policy surface it exists for. **C** stays rejected.

# Spike: publisher ergonomics face-off (issue #9)

> **Fairness review (2026-09): the "Fairness review" section at the end supersedes the evidence table.** The
> first round compared runtime-bound alternatives with static-address hand-written code, and every alternative
> stored its output by reference.

Pattern B's zero-cost publish requires either a publisher templated on its output (`template<class Out>`)
or one indirect call through `sub0x::Sink<T>`. This spike faces off coding patterns for the templated case
to see whether any of them hide the `template<class Out>` boilerplate without adding cost, and re-confirms
`Sink<T>`'s cost against the same scenario.

**Case:** `tests/collapse/cases/publisher_ergonomics/` — copy of `multi_receivers` (controllerA gain 3,
controllerB gain 5, logger; dispatch order A, B, logger; handwritten reference = direct calls). All 6
variants pass ctest (checksum-identical to handwritten, both forms). New helper: `sub0x::Publisher<Derived,
Out>` (CRTP mixin) added to `tests/collapse/sandbox/sub0x_static.hpp`, additive only.

## The alternatives

**1. Baseline — `template<class Out>`, member reference (today's B1)**
```cpp
template<class Out>
struct Sensor {
    explicit Sensor(const Out& o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    const Out& out;
};
// Sensor<Bus> sensor(bus);
```

**2. CRTP mixin — `sub0x::Publisher<Derived, Out>`**
```cpp
template<class Out>
struct Sensor : sub0x::Publisher<Sensor<Out>, Out> {
    using sub0x::Publisher<Sensor<Out>, Out>::Publisher;
    void send(uint32_t v) noexcept { this->publish(Sample{v}); }
};
```

**3. CTAD via factory — `make_sensor(bus)`**
```cpp
template<class Out>
struct Sensor { explicit Sensor(const Out& o) noexcept : out(o) {} /* ... */ const Out& out; };
template<class Out> auto make_sensor(const Out& out) noexcept { return Sensor<Out>(out); }
// auto sensor = make_sensor(bus);   -- Out/Sensor<Out> never spelled by the caller
```

**4. Composition-point argument — `Out` passed per call, not stored**
```cpp
struct Sensor {   // not a template at all
    template<class Out>
    void send(uint32_t v, const Out& out) noexcept { out.publish(Sample{v}); }
};
// sensor.send(v, bus);
```

**5. `Sink<T>` type-erased port (B3, unchanged)**
```cpp
struct Sensor {
    explicit Sensor(sub0x::Sink<Sample> o) noexcept : out(o) {}
    void send(uint32_t v) noexcept { out.publish(Sample{v}); }
    sub0x::Sink<Sample> out;
};
```

**6. Direct coupling to one static topology (control)**
```cpp
using Bus = sub0x::StaticWiring<&controllerA, &controllerB, &logger>;
struct Sensor {   // not a template; no Out anywhere
    void send(uint32_t v) noexcept { Bus::publish(Sample{v}); }
};
```

C++20 concepts were not tried separately: none of 1-4 need a constraint beyond what `Out::publish<T>`
already provides, so a `Publishable<Out,T>` concept would only improve diagnostics, not cost — noted under
ergonomics below rather than as a 7th variant.

## Evidence (gcc-O2 / clang-O2, `publisher_ergonomics`; deltas vs. handwritten, observable form)

| Variant | gcc publish | gcc setup | gcc RAM | clang publish | clang setup | clang RAM | indirect calls | cm33 text/RAM |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 baseline_template | +9 | +8 | +40 B | +0 | +0 | +0 | 0 | +20 B / +16 B |
| 2 crtp_mixin | +9 | +8 | +40 B | +0 | +0 | +0 | 0 | +20 B / +16 B |
| 3 ctad_factory | +9 | +8 | +40 B | **+5** | **+8** | +32 B | 0 | +20 B / +16 B |
| 4 call_site_out | +9 | +6 | +24 B | +0 | +0 | +0 | 0 | +12 B / +12 B |
| 5 sink_typeerased | +21 | +10 | +40 B | +11 | +8 | +32 B | 1 | +52 B / +20 B |
| 6 static_bound | +0 | +0 | +0 | +0 | +0 | +0 | 0 | +0 B / +0 B |

Full data (removable form, teardown, cm33-Os both forms, largest-symbol breakdowns): `/tmp/ergo_report.md`
from `python3 tests/collapse/collapse_evidence.py --case publisher_ergonomics` (not checked in; rerun to
regenerate). Only variant 6 passes every criterion on every build; 1/2/4 pass on Clang but not GCC; 3 is
worse than 1 on Clang (see below); 5 matches the earlier B3 result (one indirect call, everything behind it
typed).

**Finding: 1, 2 and 4 are cost-identical on GCC, and 1/2/4 collapse fully on Clang.** The CRTP mixin (2)
costs exactly what the hand-spelled baseline (1) costs on both compilers, in every build — the mixin is
free. Composition-point argument passing (4) is slightly cheaper than both in RAM (it stores no `Out&`
member at all) and matches them in instructions. GCC keeps `sub0x::Wiring`'s tuple-of-references (the RAM
delta comes from the runtime-bound `bus` object itself, not from how the publisher spells its output) and a
few extra setup instructions building it; Clang fully removes it in this single-TU case. This is the known
B1-vs-B2 cost (a runtime-bound wiring keeps addresses in RAM; a static one doesn't) — restated here to show
it is unaffected by which of 1/2/4 the publisher uses.

**Finding: the factory indirection (3) is not free on Clang.** GCC treats 3 identically to 1 (guaranteed
copy elision applies as expected), but Clang fails to fold the `make_sensor` call the way it folds the
in-place constructor call in 1/2/4, leaving `+5` publish instructions and `+8` setup instructions it
otherwise removes. The user-visible code is a one-line factory function; the cost is a compiler-specific
optimizer gap, not a language guarantee — a real risk for a helper meant to *reduce* what users write.

**Finding: 6 confirms nothing new, but is the actual zero-cost answer when the topology is static.** It
restates B2 without a template at all: name the concrete `Bus` alias in the publisher and call
`Bus::publish(msg)` directly. This is not a new pattern from this spike, but it settles where the ergonomic
line should be drawn (see recommendation).

## Ergonomics assessment

| | writes | compile errors | cross-TU/library | multi-type publisher |
|---|---|---|---|---|
| 1 baseline | `template<class Out>` + `Out&` member + ctor | on missing `.publish` member; message-agnostic | needs `Out`'s concrete type visible in the publisher's TU | fine — `this->publish` per message type, unlimited |
| 2 CRTP | `template<class Out>`, base-class list, `using Publisher::Publisher` | same underlying error, harder to read through the mixin's `publish` | same as 1 | fine |
| 3 CTAD factory | one factory template; call site drops all type spelling (`auto`) | best call-site errors (no template arg to get wrong) but a confusing error if the factory itself is misused | same as 1; the factory must also be visible | fine |
| 4 call-site arg | one non-template class, one templated method | error only at the call site, points at the actual call | same as 1, deferred to every call site instead of the ctor | fine, but Out is repeated at every call |
| 5 Sink\<T\> | non-template class holding `Sink<T>` | best decoupling: publisher TU only needs `T`, not any wiring type | genuinely crosses the boundary — the only one of the six that can | needs one `Sink<T>` member per message type published |
| 6 static coupling | no template, names `Bus` directly | clearest possible error (concrete types throughout) | cannot cross a TU/library boundary without editing the publisher | fine, but locks the publisher to one topology |

None of 1-4 removes the fundamental requirement that a publisher's own translation unit must see the
concrete `Out` type — they only change *how* that requirement is spelled. A C++20 concept on `Out` would
sharpen the error at instantiation (`Out models Publishable<Sample>` vs. a raw "no member `publish`") but
changes none of the above; not worth a second C++ standard for a SFINAE-quality-of-life gain that a
`static_assert` can give under C++17 too.

## Recommendation

**Primary: CRTP mixin (`sub0x::Publisher<Derived, Out>`, alternative 2)** for publishers whose topology is
bound at runtime (B1) or is otherwise not known when the publisher class is written. It is measured
cost-identical to hand-writing `template<class Out>` + `Out&` on every build, in every form, gives
`this->publish(msg)` instead of `out.publish(msg)`, and — unlike the factory (3) — has no compiler-specific
optimizer dependency. Ship it in `sub0x_static.hpp` (already done by this spike) as the standard way to
write a B1 publisher.

**Runner-up: composition-point argument (alternative 4)** when a publisher is reused against more than one
wiring (or tested against a fake one) — it is the cheapest of the templated forms in RAM (no stored `Out&`)
and equally cheap in instructions, at the cost of repeating the target at every call site.

**Where the topology is fixed at compile time, prefer alternative 6 (a non-template publisher naming the
`StaticWiring` alias directly) over any templated form** — it is the only alternative that is exactly
handwritten code on every build (GCC included), and it is what B2 already recommends; this spike's job was
to confirm no ergonomic wrapper beats it, not to replace it.

**Keep `Sink<T>` (5) for the one case none of the others solve:** a publisher that must be written and
compiled without seeing the application's wiring type at all (a genuine library/TU boundary). Its one
indirect call per publish is the explicit, measured price of that decoupling, unchanged from Phase 1.

**Do not adopt the CTAD-factory pattern (3)** as the recommended sugar: it is not more concise than the
CRTP mixin at the call site that matters (construction), and it is demonstrably not free on Clang.


## Fairness review (2026-09)

The first round compared alternatives 1-5 (runtime `Wiring`) with `handwritten`, which calls statically placed
receivers directly, so each delta mixed the price of runtime binding (paid by hand-written code too) with the
spelling's own cost. Every alternative also stored its output as `const Out&` next to a separate wiring object
(two hops per receiver). Corrections:
- **Equal-work references.** `handwritten_runtime`: the publisher holds the receivers' addresses, stored at
  setup. `handwritten_erased`: a C-style context pointer plus function pointer, for `Sink<T>`. Alternatives 1-4, 7
  and 8 are judged against the first, alternative 5 against the second, alternative 6 (static) against `handwritten`.
- **Best form.** The publisher holds its output by value (a `Wiring` is a tuple of receiver references, a
  `StaticWiring` is empty), including the `sub0x::Publisher` mixin; no separate wiring object where the
  alternative does not need one. `Wiring` delivers through an index sequence instead of `std::apply`.

| Alternative | gcc-O2 | clang-O2 | cm33-gcc-Os |
|---|---|---|---|
| 1 hand-spelled `template<class Out>` | = | = | = |
| 2 CRTP mixin `Publisher<Derived, Out>` | = | = | = |
| 3 CTAD factory | = | publish +4.0, path +5, RAM +24 B | text +4 B |
| 4 call-site argument | = | = | = |
| 5 `Sink<T>` (vs `handwritten_erased`) | = | = (publish -1; the static path counts inlined work) | = |
| 6 name the `StaticWiring` alias (vs `handwritten`) | = | = | = |
| 7 deducing-this mixin (C++23) | not built (GCC 13) | = | not built |
| 8 deducing-this call site (C++23) | not built | = | not built |

"=" is PASS on every criterion in both forms (publish within the harness tolerance of +2). The price of the
job itself, against static code: runtime binding costs gcc +8 publish instr and +24 B RAM in this case (clang
proves the stored addresses constant and removes it); type erasure costs gcc +22, clang +12.

**Why the factory still costs on Clang:** with alternative 1 Clang proves the sensor is only ever built from the
addresses of statics and folds the loads away; built through `make_sensor`'s by-value return, it cannot, and the
addresses stay runtime loads. A real, compiler-specific cost of the spelling.

**Revised recommendation.** Unchanged in order, stronger in substance: name the `StaticWiring` alias where the
topology is static; otherwise the CRTP mixin (or a hand-spelled template, or a call-site argument) costs exactly
what hand-written runtime binding costs; `Sink<T>` costs exactly what hand-written type erasure costs. Only
the CTAD factory carries a cost of its own (Clang), so it stays rejected. The earlier K11 is withdrawn.

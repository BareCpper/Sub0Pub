# Spike: static-path cancellation (issue #9 open item)

**Question.** Pattern B (`tests/collapse/sandbox/sub0x_static.hpp`) has no way for a receiver to stop the
rest of the current publication, unlike the runtime path's `sub0::Publish<Data>::cancel()` /
`sub0::Subscribe<Data>::cancel()`. Can a static-path receiver get the same power without static-path
receivers that never cancel paying anything for it?

**Method.** Same as [COLLAPSE_EVIDENCE.md](../COLLAPSE_EVIDENCE.md): a new case, `cancellation`
(`tests/collapse/cases/cancellation/`), with a hand-written reference and one variant per alternative,
measured by `collapse_evidence.py` on gcc-O2, clang-O2 and cm33-gcc-Os, plus a re-run of `multi_receivers`
to confirm the header changes cost the non-cancelling cases nothing.

**Case.** Receivers in bound order: `Gate` (does its own work, then decides to cancel when
`value % 3 == 0`), `Controller` (gain 3), `Logger`. Reference: direct calls with an early return after
Gate's decision, so Controller and Logger are simply never called for a cancelled publication.

## Alternatives faced off

All four are additive to `sub0x_static.hpp`; none of them touches `StaticWiring::publish`/`Wiring::publish`
or the existing `deliver`/`deliverExcept` helpers, so the non-cancelling cases keep their existing codegen
path untouched (only new, unrelated symbols are added to the header).

| # | Alternative | User code (Gate) | Non-cancelling receiver's code |
|---|---|---|---|
| 1 | **Bool return**, compile-time detected | `bool receive(const Sample& s) noexcept { ...; return v%3!=0; }` | unchanged (`void receive(...)`) |
| 2 | **Cancellation token** parameter | `void receive(const Sample& s, sub0x::Delivery& d) noexcept { ...; if (v%3==0) d.stop(); }` | unchanged (`receive(const T&)`, never sees the token) |
| 3 | **Thread-local publish context**, mirrors runtime `cancel()` | `void receive(const Sample& s) noexcept { ...; if (v%3==0) sub0x::cancel(); }` | unchanged, but the wiring still checks the TLS flag after every delivery |
| 4 | **Filter-based control** (no first-class cancel; the control) | `void receive(const Sample& s) noexcept { ...; stopped.value = (v%3==0); }` | must add `bool filter(const T&) const { return !stopped.value; }` and take a `const Stopped&` |

Alt 1 and 2 route through a fold expression that short-circuits on the first "stop" (`(deliver(...) && ...)`);
Alt 3 checks a `thread_local bool` after every delivery, saved/restored around the publication (same
discipline as `sub0pub.hpp`'s `Broker::publish`); Alt 4 needs no new header code at all — it repurposes the
already-existing `filter()` mechanism and pushes the bookkeeping onto every downstream receiver.

Call sites: `Bus::publishCancelable(msg)` (1), `Bus::publishWithToken(msg)` (2), `Bus::publishCancelableTLS(msg)`
(3), plain `Bus::publish(msg)` (4) — all `StaticWiring` (pattern B2), same as the existing acceptance cases.

## Evidence: `cancellation`, observable-work form

Deltas against handwritten (same build and form). Full report: `tests/collapse/collapse_evidence.py --case cancellation`.

| Build | Alt1 bool | Alt2 token | Alt3 TLS | Alt4 filter (control) |
|---|---|---|---|---|
| gcc-O2 publish instr | **-3.0** | **-3.0** | +0.0 | +6.3 |
| gcc-O2 text / data+bss | +0 / -8 B | +0 / -8 B | +16 B / -7 B | +64 B / +32 B |
| clang-O2 publish instr | **=** | **=** | **=** | +4.3 |
| clang-O2 text / data+bss | +0 / +0 | +0 / +0 | +0 / +0 | +32 B / +16 B |
| cm33-gcc-Os path instr | **-7** | **-7** | +7 | +8 |
| cm33-gcc-Os text / RAM | -24 B / -4 B | -24 B / -4 B | **+12 B / +253 B** | +28 B / +12 B |
| cm33-gcc-Os added deps | none | none | **TLS** | none |

(Removable-work form tells the same story; Alt 1/2/3 are within -3..+0 instr of handwritten on every build,
Alt 4 is +1..+2. Negative deltas mean the alternative's fold-based structure lets the compiler collapse it
*below* the hand-written baseline — still behaviourally identical, checksums match exactly on every
variant/build/form.)

**Alt 1 (bool) and Alt 2 (token) are indistinguishable from each other on every build**, and both come out
at or below handwritten on every criterion, including Cortex-M33 where they're smaller than the reference.
**Alt 3 (TLS) is free on gcc-O2 and clang-O2** — with everything in one TU the optimiser proves the flag
access away, exactly as `sub0pub.hpp`'s own thread_local does for pattern A's best case — **but on
Cortex-M33 it reintroduces the TLS runtime dependency** (`__aeabi_read_tp`, a 256 B `tlsBlock`) that
[COLLAPSE_EVIDENCE.md](../COLLAPSE_EVIDENCE.md) Phase 0 finding 3 flagged as a per-image bare-metal cost of
pattern A. That is the single most important number in this spike: a design that is free on the host and
expensive on the target the static path exists for. **Alt 4 (filter control) costs the most everywhere**
(+1 to +8.3 instr, +28 to +64 B text) because every downstream receiver must carry a reference to the shared
flag and pay its own `filter()` check, whether or not it would ever be the last receiver reached.

## `multi_receivers` — no regression

Re-run after the header changes: `sub0x_b2_static` (pattern B2, the case this spike shares its wiring form
with) is still **PASS on every criterion, every build, both forms** — `+0` on publish instr, text and
data+bss, identical checksum, "=" against handwritten exactly as in the Phase 1 report. `sub0x_b1_wire` and
`sub0x_b3_sink` show their pre-existing (unrelated) deltas, unchanged. The four cancellation alternatives
add new methods to `StaticWiring`/new free functions; they do not alter `publish()`, `publishFrom()`, or any
existing `detail::` helper, so every prior acceptance case keeps its measured numbers.

## Cost for non-cancelling receivers

Precisely the design goal (a): a `void receive(const T&)` receiver is unaffected by Alt 1 or Alt 2 — the
`if constexpr` on `receive_result_t`/`accepts_token` resolves at compile time to the same code as today's
`deliver()`. Alt 3 costs a TLS read (and a save/restore per publication) that the host optimiser removes
when the whole call graph is visible, but that a bare-metal image cannot always avoid. Alt 4 makes every
downstream receiver's cost visible in its own source (a `filter()` call), and it is never zero even for a
receiver that will never actually be skipped.

## Semantics vs. the runtime path's `cancel()`

`sub0pub.hpp`'s `Broker<Data>::cancel()` sets a per-Data-type `thread_local bool`, saved/restored around
`Broker<Data>::publish()` (lines 542–583). That gives: (a) cancel affects only the current publication, (b)
only receivers later in the (snapshot) subscription order are skipped, (c) nested publications are
independent because save/restore is stack-disciplined, (d) — but "two independent wirings" isn't a runtime
concept: `Broker<Data>` is one global singleton per `Data` type, shared by every `Publish<Data>`/
`Subscribe<Data>` in the program.

| Property | Runtime `cancel()` | Alt 1 bool | Alt 2 token | Alt 3 TLS | Alt 4 filter |
|---|---|---|---|---|---|
| (a) current publication only | yes (TLS, save/restore) | yes (stack-local return chain) | yes (stack-local `Delivery`) | yes (TLS, save/restore, explicit) | yes (shared flag, reset per publish if the app resets it) |
| (b) only later receivers skipped | yes | yes | yes | yes | yes, but *seen* (filter() still runs) |
| (c) nested publications independent | yes | yes, trivially (no shared state) | yes, trivially | yes, by explicit save/restore | only if the app resets the flag on entry |
| (d) two independent wirings (same type) don't interfere | n/a (no such concept at runtime) | yes, trivially | yes, trivially | yes — same save/restore discipline handles reentrant/nested wirings of the same type | yes — flag is per-application-object, not global |
| skip receiver entirely vs. "see it, do nothing" | receive() not called | receive() not called | receive() not called | receive() not called | `receive()` not called but `filter()` **is** called |
| toolchain cost model | TLS always (pattern A finding 3) | none beyond the receiver's own branch | none beyond the receiver's own branch + one stack local | none on host; TLS revives on bare metal | one branch per downstream receiver, always |

Alt 1/2/3 all reproduce (a)–(d) faithfully; Alt 1 and 2 do it with *no* shared or thread-local state at all
(strictly better than the runtime path's structure), because a static-path publication's receiver set is
already known at compile time — there is no registry to snapshot. Alt 3 reproduces the runtime path most
literally (same TLS + save/restore idiom) and inherits its one weakness on embedded targets.

## C++23 note (maintainer input)

Asked to check whether a C++23 feature would materially simplify this. **`deducing this`** and **`if
consteval`** don't apply — the compile-time detection here is ordinary C++17 SFINAE (`std::void_t`), which
already works at zero cost; nothing about them makes `accepts`/`accepts_token` simpler or cheaper.
**`std::expected`** does help expressively: a `sub0x::Stop` variant of Alt 1 lets `receive()` return
`std::expected<void, sub0x::Stop>` instead of a bare `bool` — a self-documenting reason for stopping,
still returned by value (no out-parameter, no shared state), detected and short-circuited the same way.

Added as **Alt 1c**, guarded entirely behind `#if defined(__cpp_lib_expected)` in `sub0x_static.hpp` (inert
under C++17 — this project stays on `cxx_std_17`; nothing here changes that). It is deliberately **not**
wired into the ctest collapse harness, which builds every case at the project's C++17 standard; wiring in a
C++23-only source file there would break that build. Instead it's a standalone demo,
`docs/design/spikes/cpp23_demo/sub0x_alt1c_expected.cpp`, built and measured by hand:

```
g++ -std=c++23 -O2 -I tests/collapse -I include tests/collapse/driver.cpp \
    docs/design/spikes/cpp23_demo/sub0x_alt1c_expected.cpp
```

Measured against `handwritten` (also rebuilt at `-std=c++23` for a fair baseline) on **gcc-O2**: checksum
identical; `.text` identical (2439 B both); `.bss` **8 B smaller** than handwritten (Gate carries no
`canceled` member — the expected's reason value replaces it); per-phase callgrind instructions setup 20
(-1), publish 27.0 (**-3.0**), teardown 16 (+0) — the same numbers, to the instruction, as Alt 1's bool
return in the automated harness. **Caveat:** on this toolchain, clang 18 built against libstdc++ 13 does
not expose `__cpp_lib_expected` (the feature-test macro is never defined for clang even though `<expected>`
parses), so Alt 1c only builds/measures with GCC here; a clang+libc++ toolchain was not available to test.
If the project moves to C++23, `std::expected` is a strict expressiveness upgrade over Alt 1's bare `bool`
at identical (GCC-measured) cost, not a new alternative with new trade-offs — everything in the "vs runtime
`cancel()`" table above for Alt 1 applies to Alt 1c unchanged.

## Recommendation

**Alt 1 (bool return)**, with **Alt 2 (token) as runner-up**.

- Both are free for non-cancelling receivers, both faithfully reproduce every semantic property of the
  runtime path's `cancel()` without needing any shared or thread-local state (a strict improvement, made
  possible because the static path's receiver set is fixed at compile time), and both measured identically
  on every build in this spike.
- **Alt 1 is simpler to write and explain** — "return `false` to stop" is one concept, no new type. It's
  the natural fit for the common case (a gate that decides yes/no).
- **Alt 2 is the runner-up** for when a receiver wants to keep a `void` return (e.g. it also needs to
  return nothing to fit some other constrained signature) or wants the token available for future extension
  (e.g. carrying a reason without changing the return type) without the C++23 dependency Alt 1c would need
  for that same expressiveness.
- **Alt 3 (TLS)** is not recommended as the default: it is free today only because these host builds see
  the whole call graph in one TU; it reintroduces exactly the bare-metal TLS cost that motivated pattern B
  in the first place (COLLAPSE_EVIDENCE.md Phase 0 finding 3), and it needs explicit save/restore discipline
  that Alt 1/2 get for free from the call stack. It stays useful only as a deliberate bridge if/when the
  static and runtime paths need to share one cancellation vocabulary (Phase 1's "next" item).
- **Alt 4 (filter control)** confirms the need for first-class support: without it, cancellation becomes
  manual, ad hoc, always-on-cost bookkeeping that every downstream receiver must remember to add.

If/when the project adopts C++23, **Alt 1c (`std::expected<void, Stop>`)** is a free upgrade path from Alt 1
with no measured cost difference (GCC) and a self-documenting stop reason; it does not change this
recommendation, only its eventual return type.

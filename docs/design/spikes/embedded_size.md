# Spike: embedded image size of today's API (issue #2)

Face-off of every source of retained code/data/link dependency that today's public `sub0pub.hpp`
(pattern A) forces onto a Cortex-M33 `-Os` image, with measured final-linked-ELF bytes per lever,
alone and combined. Levers are implemented as `SPIKE_*` macros on a **copy** of the header
(`tests/collapse/sub0pub_variants/sub0pub_spike.hpp`); the public header is untouched.

## Method

- **Cases:** `tests/collapse/cases/{zero,one,multi}_receivers`, each case's existing `sub0pub_virtual.cpp`
  (today's header) plus a new `sub0pub_spike.cpp` (lever header), built by `collapse_evidence.py`'s
  discovery (any `*.cpp` in a case dir is a variant) and confirmed bit-identical to `sub0pub_virtual` at
  default macro settings (same checksum, same instruction/byte counts).
- **Build:** `arm-none-eabi-g++ 13.2.1 -Os -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16
  -fno-exceptions -fno-rtti --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections`, final linked ELF
  (driver.cpp + variant + bare-metal TLS stub), via a new script,
  `tests/footprint/spike/run_embedded_size_spike.sh` (not part of `ctest`; a one-off spike tool).
- **Correctness check:** the full `tests/test_pubsub.cpp` suite (7 test cases, 16 assertions) recompiled
  against the lever header through a shadow include dir (`tests/footprint/spike/shadow_include/`), at
  default settings and at the non-virtual-destructor lever — both pass unmodified (filter-non-virtual
  breaks `test_pubsub.cpp`'s `filter() override`, expected — see lever H below).
- `ctest --preset default`: 69/69 pass, including the two new `sub0pub_spike` variants added to every
  existing collapse case (host + cm33, observable + removable forms).

## Inventory of cost sources (today's header, default macros, `-fno-exceptions -fno-rtti`)

| Source | Where | Cost |
|---|---|---|
| `virtual ~Subscribe()` / `virtual ~Publish()` | sub0pub.hpp:230, 331 | vtable entries + a deleting-destructor thunk that references `operator delete`, pulling `_ZdlPv` and (via nano.specs) `malloc`/`_sbrk`/newlib heap plumbing into images that never allocate. Confirms `docs/design/COLLAPSE_EVIDENCE.md` Phase 0. |
| `virtual bool filter()` | sub0pub.hpp:238 | second indirect call per subscriber per publish, paid by every subscriber whether or not it overrides `filter()` (matches `PERFORMANCE_BASELINE.md` finding 4). |
| `thread_local` publish context (`threadCurrent_`, `threadCanceled_`) | sub0pub.hpp:631-632 | unconditional, not gated by `SUB0PUB_THREAD_SAFE`; requires `__aeabi_read_tp`, i.e. a working TLS model even on a single-core, single-thread target (`PERFORMANCE_BASELINE.md` finding 2). |
| `typeid`/`__PRETTY_FUNCTION__` type names | `typeHash<T>()`, sub0pub.hpp:706-720 | `constexpr`, not RTTI (`-fno-rtti` compiles fine) and not retained at default settings — **no cost** at `SUB0PUB_TYPEIDNAME=0` (its default). |
| `std::copy_n` snapshot | sub0pub.hpp:562 | only compiled under `SUB0PUB_REENTRANT_SAFE` (default on); lowers to inline `memcpy`, no extra symbol at `-Os` for the small (8-entry) table. |
| `SUB0PUB_STD` / iostream | sub0pub.hpp:113-116 | off by default; with `-Wl,--gc-sections` and no call site actually using `OStream`, enabling it alone adds **0 bytes** to these cases (confirmed below) — the iostream *implementation* is never pulled in unless something calls it. |
| RTTI (compiler flag, issue #2's literal ask) | n/a | today's header itself never uses `typeid`/`dynamic_cast`, so compiling **with** RTTI enabled (dropping `-fno-rtti`, issue #2's scenario) doesn't add RTTI *machinery* Sub0Pub needs — but every polymorphic class (`Subscribe<T>`, any user subscriber) now gets a full `type_info` object with mangled-name string instead of the RTTI-off short form, which is strictly bigger. Measured below. |

## Lever results (final-linked ELF, bytes; `text` / `data` / `bss`)

`zero_receivers` = 1 publisher, 0 subscribers. `one_receiver` = 1 publisher, 1 subscriber (`Controller`).
`multi_receivers` = 1 publisher + 3 subscribers across 2 distinct types (`Controller` used twice via CRTP
subclass instances is not tested; these are 2 distinct receiver *classes*, `Controller` and `Logger`, so
the delta here is dominated by per-type vtable/typeinfo, not by "a second instance of the same type").

| Lever | zero text/data/bss | one text/data/bss | multi text/data/bss | Link deps added |
|---|---|---|---|---|
| **A. Today (default macros)** | 3720/88/821 | 5452/88/837 | 5812/88/861 | TLS, operator delete, malloc/sbrk |
| B. Today, **RTTI enabled** (drop `-fno-rtti`, issue #2's exact ask) | 5008/88/821 (**+1288**) | 6836/88/837 (**+1384**) | 7236/88/861 (**+1424**) | same, plus full `type_info`/mangled names |
| C. `SUB0PUB_TYPEIDNAME=1` | **BUILD FAILS** — see bug below | | | |
| D. `SUB0PUB_STD=1` (iostream path compiled, unused) | 3720/88/821 (+0) | 5452/88/837 (+0) | 5812/88/861 (+0) | none added — confirms no-cost-if-unused |
| E. Non-virtual `~Subscribe()` alone | 3716/88/821 (−4) | 5400/88/837 (−52) | 5720/88/861 (−92) | unchanged (Publish still pulls operator delete) |
| F. Non-virtual `~Publish()` alone | 2955/8/805 (**−765/−80/−16**) | 5296/88/829 (−156) | 5656/88/853 (−156) | operator delete/malloc gone at zero_receivers (no live Publish vtable use); still present where a Subscribe vtable remains |
| G. Both non-virtual dtors | 2951/8/805 (−769/−80/−16) | 4632/8/813 (**−820/−80/−24**) | 4952/8/837 (**−860/−80/−24**) | operator delete, malloc/sbrk gone entirely; TLS remains |
| H. G + non-virtual `filter()` | 2959/8/805 | 4608/8/813 (−24 more) | 4924/8/837 (−28 more) | same as G |
| I. TLS → plain `static` (single-threaded only) | 3660/88/568 (bss −253) | 5396/88/580 (bss −257) | 5752/88/604 (bss −257) | TLS symbol gone; text ~flat (TLS access was cheap), **bss drops ~253-257 B** (the TLS block/model overhead, not the 5 B of payload) |
| **J. G+H+I combined** | **2903/8/548** (**−817/−80/−273** vs A) | **4556/8/560** (**−896/−80/−277**) | **4868/8/584** (**−944/−80/−277**) | **no TLS, no operator delete, no malloc/sbrk, no pure-virtual** |
| K. `SUB0PUB_THREAD_SAFE=1` | **BUILD FAILS** | | | confirms `PERFORMANCE_BASELINE.md` finding 1 |

Marginal cost of 2 extra (distinct-type) subscribers, lever A: +360 text / +24 bss (one→multi). Same
delta under lever J: +312 text / +24 bss — the per-subscriber marginal cost (mostly a second vtable +
typeinfo-name for the second concrete type, `Logger`) does **not** shrink much from the dtor/filter
levers, because it's dominated by per-*type* vtable/typeinfo, not by the mechanism issue #2 flags. The
"~4 instructions per subsequent subscriber" issue #2 describes is a **dispatch-loop** cost (already O(1)
per entry, see `Broker<T>::publish()`, sub0pub.hpp:542-583), not a link-size one; it's not reproduced as
a size regression here and is already tracked as a runtime-cost item in `PERFORMANCE_BASELINE.md`.

## Two bugs found by this spike (not previously exercised)

1. **`SUB0PUB_TYPEIDNAME=1` fails to compile**, with or without `SUB0PUB_STD`. `Subscribe<Data>` (line
   273) and `Publish<Data>` (line 378) declare `friend OStream& operator<<(...)` when
   `SUB0PUB_TYPEIDNAME` is on, but `OStream` is a typedef declared far later in the file (line
   ~1146-1151, in the IPC section) — a forward-reference the compiler rejects (`'OStream' does not name
   a type`). This configuration is untested (`grep -rn SUB0PUB_TYPEIDNAME tests/` finds nothing besides
   this spike) and has presumably never compiled. Filed as a follow-up; out of scope for this spike to
   fix, since it means today's typeHash/typeName diagnostics path cannot currently be measured or used at
   all with `SUB0PUB_TYPEIDNAME=1`.
2. **`SUB0PUB_THREAD_SAFE=1` does not compile on arm-none-eabi-g++ 13** (`nano.specs`/`nosys.specs`):
   `std::mutex` is unavailable because newlib-nano's libstdc++ has no gthreads model on bare metal.
   Already documented in `PERFORMANCE_BASELINE.md` finding 1; reconfirmed here at the ELF-link level
   (compile failure, not just a documented risk).

## Lever table: bytes saved, impact, migration, v2 fit

| Lever | Bytes saved (one_receiver, text+data+bss) | API / semantic impact | Migration impact | v2 fit |
|---|---|---|---|---|
| Non-virtual, protected `~Subscribe()`/`~Publish()` | ~900 (G vs A) | None observable: neither type is ever `delete`d through a base pointer today (broker only calls `receive()`/`filter()`); protected dtor still allows normal derived-object destruction. Only breaks code that does `Subscribe<T>* p = new Derived; delete p;` (polymorphic ownership through the base) — not a documented/supported pattern. | Source-compatible for all current tests/examples; would be a **binary-incompatible, source-compatible** ABI change (vtable layout shrinks). MIGRATION.md entry: "Subscribe<T>/Publish<T> destructors are no longer virtual; do not delete through a base pointer." | **Recommended for v2's static hot-path base.** Pattern B's CRTP/static wiring never needs a virtual base at all; this lever is the natural bridge for any runtime-registry boundary type that must stay ABI-stable but shouldn't force a vtable-only-for-cleanup cost. |
| `filter()` non-virtual by default (opt-in via CRTP/detection idiom, as `tests/design/broker_config/sub0x_broker.hpp` already prototypes) | ~24-28 additional | Breaking: `override` on `filter()` no longer compiles against the base; needs a detection-idiom base (already built and tested in the sub0x prototype) so filter is a *capability*, not a tax. | Requires a real API change (removing the virtual from the public `Subscribe<Data>`), so **is** a MIGRATION.md-worthy breaking change if ever landed on `sub0pub.hpp` directly — but the v2 direction replaces this class entirely, so the migration path is "adopt the v2 registry type," not a point patch. | **Already solved in the v2 direction**; this spike just quantifies the win (modest — filter's per-type cost is small next to the dtor/vtable win). |
| TLS → plain `static` when single-threaded | ~250-280 B bss, no text change | Loses re-entrant `cancel()` correctness across real threads; fine for the common single-core embedded case, wrong for an RTOS running publish on two cores/threads. Needs to be a configuration axis (today's `SUB0PUB_THREAD_SAFE` already distinguishes single vs multi-threaded builds; TLS-vs-static should follow the same knob) rather than a silent default change. | Config-macro-gated (`SUB0PUB_` prefixed per STYLE_GUIDE.md), no signature change; MIGRATION.md not required unless the *default* changes. | Fits the runtime-registry boundary: a boundary type that is explicitly single-core (a common embedded deployment) can opt out of TLS entirely; the static hot-path types shouldn't need publish-context state at all if cancel() is redesigned per-callsite rather than thread-global. |
| RTTI on vs off | Not a lever Sub0Pub controls — but today's header pays **+1.3 KB per case** on Cortex-M33 the moment a user's project (or a library they link) needs RTTI anywhere. | None — this is a project-wide compiler flag, not something the library can force locally without `-fno-rtti`-scoped attributes (not universally portable). | No header change possible; **documentation** fix: state plainly in the embedded guidance that `-fno-rtti` is the expected default and quantify what enabling it costs, so users aren't surprised when another dependency forces RTTI on. | Not a v2 registry change; a README/porting-guide addition. |
| `SUB0PUB_TYPEIDNAME`, `SUB0PUB_STD` at their documented defaults (off) | Already 0 (confirmed lever D) | None — already free when unused. | None. | No action needed; document that these levers are correctly free today, the opposite of issue #2's report of "iostream elements that cannot be removed." |
| Fix `SUB0PUB_TYPEIDNAME`'s `OStream` forward-reference bug | N/A (currently unusable) | Fixes a latent compile break, not a size question | Small, localized (move the `OStream` typedef earlier, or forward-declare it) — flag for a normal bugfix commit, separate from this spike. | Needed regardless of v2 direction if anyone tries to use diagnostics on today's header. |

## What issue #2 can be closed by

Issue #2 specifically calls out "RTTI and iostream elements that cannot be removed... roughly 4
instructions per subsequent subscriber". This spike finds:

- **iostream is already removable** — `SUB0PUB_STD=0` (the default) adds zero bytes when unused; issue
  #2 predates or doesn't reflect the current header's `#if SUB0PUB_STD` guards. No further action needed
  there beyond documenting it.
- **RTTI** is not consumed by the header's own logic (no `typeid`, no `dynamic_cast`); the cost issue #2
  observed is the *vtable/typeinfo overhead of any polymorphic type* when a project globally enables
  RTTI, which is standard C++ cost, not Sub0Pub-specific. Closeable by documentation (state the
  `-fno-rtti` expectation) plus the non-virtual-destructor lever, which shrinks what's paid **even when**
  RTTI is enabled (fewer polymorphic types with a vtable at all → less typeinfo).
- **"~4 instructions per subsequent subscriber"** is a dispatch-loop runtime cost, not link size; it is
  not reproduced as a size regression in this spike (marginal size per extra subscriber barely moves
  between lever A and lever J) and should be tracked/verified separately in `docs/PERFORMANCE_BASELINE.md`
  (it may already be resolved there — the loop is a flat array iteration with one indirect call per
  entry, i.e. O(1) per subscriber, not accumulating "4 instructions extra" per subsequent one; worth a
  follow-up instruction-count check against that exact wording).

**Recommendation:** close issue #2 by (a) landing the non-virtual protected destructor change on
`Subscribe<T>`/`Publish<T>` (the single biggest lever: ~900 B / ~15-16% of a one-subscriber Cortex-M33
image, and it removes `operator delete`/`malloc`/`_sbrk` from images that never allocate — the exact
class of dependency issue #2 flags), (b) documenting that `SUB0PUB_STD`/iostream are already free when
unused and that `-fno-rtti` is the expected embedded default, with the measured RTTI-on cost quoted, and
(c) filing the `SUB0PUB_TYPEIDNAME` `OStream`-ordering bug separately. Making TLS opt-out and `filter()`
non-virtual-by-default are both real wins but are breaking API changes best delivered as part of the v2
registry type (pattern B), not as patches to today's `sub0pub.hpp`.

## C++23 (maintainer note: possible future toolchain)

`arm-none-eabi-g++` 13.2.1 (currently installed) accepts `-std=c++23` and compiles cleanly for
Cortex-M33, but its C++23 support is partial — checked directly against this spike's concerns:

- **"Deducing this" (P0847, explicit object parameters)** — the feature most relevant to this spike,
  since it can replace the CRTP-without-vtable pattern (`sub0x_b2_static`/pattern B already use CRTP, not
  virtuals) with a single non-template-parameterized member and no base-class inheritance at all, and
  could let a single `filter`/`receive` write serve both "has an override" and "does not" shapes without
  the detection-idiom machinery in `sub0x_broker.hpp`. **Not supported by GCC 13** (host or
  arm-none-eabi) — confirmed by compiling `void f(this Foo& self)` under `-std=c++23`: rejected
  (`expected identifier before 'this'`). GCC's deducing-this support landed in **GCC 14**. If the project
  adopts C++23, this lever is blocked until the arm-none-eabi toolchain is upgraded past 13.
- **`if consteval`** — supported by GCC 13 (confirmed compiling under `-std=c++23 -mcpu=cortex-m33
  -fno-exceptions -fno-rtti`). Useful for `typeHash<T>()`/`hash()`: could let the same function have a
  guaranteed-compile-time path (for the fixed table sizing / fingerprinting already done via `constexpr`)
  and an explicit runtime fallback without relying on the "may or may not constant-fold" behaviour
  `constexpr` alone gives today — low priority, since the existing `constexpr` functions already fold
  when used in constant contexts and are unused otherwise (see the inventory above: no cost at
  `SUB0PUB_TYPEIDNAME=0`).
- **`static operator()`** (P1169) — would let a stateless functor-style receiver (e.g. the free-function
  sink style in `sub0x_b3_sink.cpp`) skip an implicit `this` pointer entirely; minor, and GCC 13's support
  for it was not independently verified in this spike (lower priority than deducing-this, which blocks
  the bigger win).

**Bottom line for the maintainer:** C++23 doesn't unlock anything actionable on the currently-installed
arm-none-eabi-g++ 13 toolchain — the one feature that would matter most for this spike's goal (deducing
this, as a vtable-free alternative to CRTP for the registry boundary) needs GCC 14+. Don't block v2's
registry design on C++23; revisit deducing-this as an optional simplification once the embedded toolchain
moves to GCC 14, and treat `if consteval` as a nice-to-have, not a blocker either way.

## Artifacts

- `tests/collapse/sub0pub_variants/sub0pub_spike.hpp` — copy of `sub0pub.hpp` with `SPIKE_SUB_VDTOR`,
  `SPIKE_PUB_VDTOR`, `SPIKE_FILTER_VIRTUAL`, `SPIKE_TLS` macros (each defaults to today's behaviour).
- `tests/collapse/cases/{zero,one,multi}_receivers/sub0pub_spike.cpp` — new variants pointing at the
  lever header; picked up automatically by `collapse_evidence.py`'s glob and `ctest`.
- `tests/footprint/spike/run_embedded_size_spike.sh` — final-ELF lever sweep for this report (not part of
  `ctest`).
- `tests/footprint/spike/shadow_include/sub0pub/sub0pub.hpp` — shadow copy used for the
  `tests/test_pubsub.cpp` correctness check.

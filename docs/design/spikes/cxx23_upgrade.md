# Spike: does C++20/23 materially improve the v2 design (issue #9)?

The maintainer asked whether Sub0Pub should move off C++17 "if there are auto semantics that may assist"
correctness-without-cost. This spike faces off C++20/23 language features against the C++17 baseline
(`tests/collapse/sandbox/sub0x_static.hpp`, `docs/design/spikes/publisher_ergonomics.md`) using the same
collapse-evidence loop (`docs/design/COLLAPSE_EVIDENCE.md`), and prices the toolchain cost of requiring
either standard.

**New harness capability (additive, C++17 variants untouched):** a variant opts into a non-default standard
by making its first source line `// SUB0X_STD: c++NN`, and may name a required language feature on its
second line, `// SUB0X_REQUIRES: <feature>`. `tests/collapse/collapse_evidence.py` (`variant_std()`) and
`tests/collapse/CMakeLists.txt` both read these markers; CMake additionally probes once per feature (via
`try_compile` at `CXX_STANDARD 23`, `SUB0PUB_COLLAPSE_HAVE_<feature>`; features `deducing-this` and
`expected`) whether the *building* compiler supports it, and skips (not fails) any variant that needs one it lacks, so `ctest` stays green on every CI
compiler. New cases/variants added by this spike:
- `tests/collapse/cases/publisher_ergonomics/alt7_deducing_this_mixin.cpp`, `alt8_deducing_this_callsite.cpp`
  (the spike first put these in a copy of the whole case; the copies were byte-identical and are folded back)
- `tests/collapse/cases/multi_receivers/sub0x_b2_static_cxx20.cpp`, `tests/collapse/cases/filters/sub0x_b2_static_cxx20.cpp`
- `tests/collapse/sandbox/sub0x_static_concepts.hpp` (concepts fork of the detection idioms, additive)

All new variants pass `ctest` (checksum-identical to `handwritten`, both forms) on GCC 13 (verified locally;
the two deducing-this variants are skipped there by design and verified separately with clang++ 18 directly).

## Headline finding

**GCC 13.3 and arm-none-eabi-g++ 13.2 reject C++23 deducing this (P0847) outright** — confirmed here and by
a sibling spike. It ships from GCC 14. Clang 18 accepts it fully. This is the toolchain fact that dominates
the recommendation below: it is not a diagnostics or codegen question, it is "does the compiler in the
embedded toolchain even parse it".

```
$ arm-none-eabi-g++ -std=c++23 ... -c dt_test.cpp
error: expected identifier before 'this'
```

C++20 concepts, by contrast, compile and produce byte-identical output on **all three** installed toolchains
(g++ 13.3, clang++ 18, arm-none-eabi-g++ 13.2), so concepts do not carry the same risk.

## Face-off 1: publisher ergonomics — deducing this vs CRTP vs `template<class Out>`

`publisher_ergonomics.md` already showed the C++17 CRTP mixin `sub0x::Publisher<Derived,Out>` is
cost-identical to hand-writing `template<class Out>` + `Out&`, on every build — but it spells the derived
class twice (`Publisher<Sensor<Out>, Out>`) even though the mixin's `Derived` parameter is never actually
used for anything (no CRTP downcast happens in its body). This spike asks: does deducing this let a
**non-template** publisher reach a concrete bus type for free, and does it at least clean up the CRTP
mixin's redundant `Derived` spelling?

**Before (C++17 CRTP, `Derived` unused but still spelled twice):**
```cpp
template<class Out>
struct Sensor : sub0x::Publisher<Sensor<Out>, Out> {
    using sub0x::Publisher<Sensor<Out>, Out>::Publisher;
    void send(uint32_t v) noexcept { this->publish(Sample{v}); }
};
```

**After (C++23 deducing this, alt7 — `publisher_ergonomics/alt7_deducing_this_mixin.cpp`):**
```cpp
template<class Out>
class Publisher23 {
public:
    constexpr explicit Publisher23(const Out& out) noexcept : out_(out) {}
protected:
    template<class T>
    void publish(this auto&& self, const T& msg) noexcept { self.out_.publish(msg); }
    const Out& out_;
};
template<class Out>
struct Sensor : Publisher23<Out> {
    using Publisher23<Out>::Publisher23;
    void send(uint32_t v) noexcept { this->publish(Sample{v}); }
};
```
The mixin drops from two template arguments to one; the explicit object parameter (`self`) replaces the
CRTP downcast the old code never did.

**Attempt at a genuinely non-template publisher (alt8, `alt8_deducing_this_callsite.cpp`):**
```cpp
struct Sensor {   // not a template
    template<class Out>
    void send(this Sensor& /*self*/, uint32_t v, const Out& out) noexcept { out.publish(Sample{v}); }
};
```
`this` in an explicit object parameter deduces the *caller's own static type* (here, always `Sensor` —
already concrete). It gives no way to learn an unrelated `Out` from `self`; `Out` still has to come from
somewhere, so `send` stays a template exactly as in the C++17 call-site-argument form
(`publisher_ergonomics/alt4_call_site_out.cpp`).

### Evidence (deltas vs. `handwritten`, `publisher_ergonomics`, observable form)

| Variant | Toolchain | publish instr | setup instr | text | verdict |
|---|---|---:|---:|---:|---|
| alt2 crtp_mixin (C++17) | gcc-O2 | +9 | +8 | +64 B | matches alt1 (baseline) exactly |
| alt2 crtp_mixin (C++17) | clang-O2 | +0 | +0 | +0 B | fully collapses |
| **alt7 deducing_this_mixin (C++23)** | gcc-O2 | **build error** | — | — | GCC 13 rejects deducing this |
| **alt7 deducing_this_mixin (C++23)** | clang-O2 | **+0** | **+0** | **+0 B** | **PASS — identical to handwritten** |
| **alt7 deducing_this_mixin (C++23)** | cm33-gcc-Os | **build error** | — | — | arm-none-eabi-gcc 13 rejects deducing this |
| alt8 deducing_this_callsite (C++23) | clang-O2 | +0 | +0 | +0 B | PASS, same shape/cost as alt4 |
| alt6 static_bound (control) | all | +0 | +0 | +0 B | still the only alternative free on GCC too |

(Removable form on clang shows a small +2 setup instr / +16 B text for alt7/alt8 relative to `handwritten`,
from the locally hand-rolled `Bus_` wiring these two files use so as not to require C++23 in the shared
C++17 sandbox header — not a deducing-this cost; the observable-form numbers above, which is what
`publisher_ergonomics.md`'s table reports for alt1–alt6, are the comparable ones.)

### Answer

**No** — a non-template publisher class cannot get zero-cost typed publication from deducing this; the
explicit object parameter deduces the *caller's* type, not an unrelated output type, so genericity over
`Out` is unavoidable for a direct call (unchanged conclusion from `publisher_ergonomics.md`). **Deducing
this is, however, a real (if narrow) improvement to the CRTP mixin's spelling**: `Publisher23<Out>` instead
of `Publisher<Sensor<Out>, Out>`, cost-identical to the C++17 mixin on Clang, at zero measured overhead.
It buys nothing static coupling (alt6) didn't already have, and it does not compile at all on the two GCC
toolchains that matter for this project today.

## Face-off 2: concepts replacing SFINAE (`accepts` / `has_filter`)

`sub0x_static_concepts.hpp` replaces `detail::accepts<R,T>` / `detail::has_filter<R,T>` (void_t SFINAE
traits) with `requires`-expressions:

**Before (C++17 SFINAE, `sub0x_static.hpp`):**
```cpp
template<class R, class T, class = void> struct accepts : std::false_type {};
template<class R, class T>
struct accepts<R, T, std::void_t<decltype(std::declval<R&>().receive(std::declval<const T&>()))>> : std::true_type {};
```

**After (C++20 concept):**
```cpp
template<class R, class T>
concept Receiver = requires(R& r, const T& t) { r.receive(t); };
```
`deliver()`'s `if constexpr (accepts<R,T>::value)` becomes `if constexpr (Receiver<R,T>)`; `has_filter`
becomes `FilteringReceiver` the same way. The silent-skip semantics (a receiver missing `receive` gets
nothing — that is by design, per `COLLAPSE_EVIDENCE.md`'s "routing by capability") are unchanged.

### Evidence: codegen (multi_receivers and filters, B2 static topology, deltas vs. C++17 `sub0x_b2_static`)

| Case | Build, form | C++17 SFINAE | C++20 concepts | Delta |
|---|---|---:|---:|---:|
| multi_receivers | gcc-O2, observable | publish 30, text 2431, RAM 632 | publish 30, text 2431, RAM 632 | **0 on every metric** |
| multi_receivers | clang-O2, observable | publish 33, text 2170 | publish 33, text 2170 | **0** |
| multi_receivers | cm33-gcc-Os, observable | path 30, text 1188 | path 30, text 1188 | **0** |
| filters | gcc-O2, observable | publish 19, text 2367 | publish 19, text 2367 | **0** |
| filters | clang-O2, observable | publish 22, text 2122 | publish 22, text 2122 | **0** |
| filters | cm33-gcc-Os, observable | path 19, text 1128 | path 19, text 1128 | **0** |

Every removable-form and `-lto` row (12 build/form combinations total, both cases) matches identically too
— the concepts variant is byte-for-byte the SFINAE variant everywhere it builds. **Codegen change: none, as
predicted.** (Full table: rerun `python3 tests/collapse/collapse_evidence.py --case multi_receivers` /
`--case filters`.)

### Evidence: diagnostics quality (captured, g++ 13.3, `-c`, `NoReceive` lacks `receive()`)

**SFINAE (`std::void_t`, C++17):**
```
error: no matching function for call to 'mustReceive(NoReceive&, Sample&)'
note: candidate: 'template<class R, class T, class> void mustReceive(R&, const T&)'
note:   template argument deduction/substitution failed:
error: 'struct NoReceive' has no member named 'receive'; did you mean 'NoReceive'?
```
The real cause (line 3, inside the `void_t` trait) is reported as if it were the trait's own body, several
indirections from the call site, with no statement of *what capability* was required.

**Concepts (C++20):**
```
error: no matching function for call to 'mustReceive(NoReceive&, Sample&)'
note: candidate: 'template<class R, class T> requires Receiver<R, T> void mustReceive(R&, const T&)'
note:   constraints not satisfied
note:   required for the satisfaction of 'Receiver<R, T>' [with R = NoReceive; T = Sample]
note:   in requirements with 'R& r', 'const T& t' [with R = NoReceive; T = Sample]
note: the required expression 'r.receive(t)' is invalid
```
The concept's name (`Receiver`) appears in the candidate signature, and the last line states the exact
expression that failed (`r.receive(t)` is invalid) — a real, measured diagnostics improvement, not
subjective: the concepts error names the missing capability, the SFINAE error does not.

### Answer

Concepts are a strict, zero-cost, zero-risk upgrade over the SFINAE idioms already in `sub0x_static.hpp` and
the #8 prototype (`sub0x_broker.hpp`'s member-alias/ADL/traits resolution uses the same shape of `void_t`
detection): identical codegen on every build measured, no toolchain gap (all three compilers accept C++20),
and a genuine diagnostics improvement for library users who get a constraint failure instead of a `void_t`
substitution error three frames removed from the actual missing member.

## Face-off 3: other candidates (brief, evidence where cheap)

| Candidate | Applies to | Finding |
|---|---|---|
| **`consteval` type ids** | `utility::typeHash<T>()` (`sub0pub.hpp:706-720`, hashes `__PRETTY_FUNCTION__`) | **Real, cheap win.** Empirically: marking `hash()`/`typeHash()` `consteval` instead of `constexpr` forces compile-time evaluation *regardless of optimisation level*. At `-O0`, the current `constexpr` version leaves a runtime `call` to `hash()` in the object file; the `consteval` version folds straight to a `mov $imm` with no call, at `-O0`. At `-O2` both are identical (single `mov`). Since Sub0Pub is header-only and used in debug/unoptimised embedded builds too, `consteval` is a free guarantee that today merely relies on the optimiser noticing. Requires C++20. Low risk, small API-invisible change (`typeHash` stays a private `utility::` function). |
| **`static operator()`** | `Sink<T>`'s `call_` member and similar dispatch points | **No measured benefit here.** `Sink<T>::call_` is already a plain (non-capturing) function pointer, the cheapest form — there is no functor object to make `static`. Where a project *does* write a stateless functor type (not a lambda-to-fnptr), `static operator()` measurably removes the hidden object parameter at `-O0` (confirmed: a hand-written stateless functor's `operator()` call passes an extra register and computes `&f` at `-O0`; a `static operator()` call does not; both fold identically at `-O2`). No such type exists in Sub0Pub today, so this is a "keep in mind for v2, not a current win". |
| **`if consteval`** | Any dual compile-time/runtime code path | Not applicable: Sub0Pub's headers have no `if (std::is_constant_evaluated())`-shaped code today, and no candidate in this spike introduced one (see `typeHash` above, which is unconditionally compile-time under `consteval`). No evidence to gather. |
| **`std::expected` for `SendResult`/`PublishReport`** | `sub0x::SendResult` (`tests/design/broker_config/test_endpoints.cpp`) | **Not a fit.** `SendResult` is already a plain enum (`Accepted`/`Full`/`Closed`/`Disconnected`) returned by value with no payload on the success path — `std::expected<void, SendResult>` would add a discriminant-plus-empty-tuple over what is already a single enum return, for no information gain. Revisit only if a success path grows a payload (e.g. bytes actually sent). |
| **`std::atomic::wait/notify`** | Quiescence waits for a runtime registry teardown | Not present in the codebase today — no spin-wait or `sleep_for`-based quiescence loop exists yet (`COLLAPSE_EVIDENCE.md`'s "static-to-dynamic bridge" and cancellation are still open work). Forward-looking only: when that bridge is built, `wait`/`notify` replace a hand-rolled spin/backoff loop with an OS-assisted block, on the same `std::atomic` the code needs anyway for the runtime path — a plausible C++20 win for the *dynamic* registry only, not for the collapse-critical static path this issue is about. No evidence to gather until that code exists. |

## Face-off 4: toolchain support matrix

| Feature | g++ 13.3 | clang++ 18 | arm-none-eabi-g++ 13.2 (`-fno-exceptions -fno-rtti`) | MSVC 19.3x/19.4x (unverified — docs) |
|---|---|---|---|---|
| C++20 concepts | **Yes** (verified) | **Yes** (verified) | **Yes** (verified) | Yes, from VS 2019 16.3 in `/permissive-` mode (unverified here) |
| C++23 deducing this (P0847) | **No** — verified reject, "expected identifier before 'this'"; lands in **GCC 14** | **Yes** (verified) | **No** — verified reject (same GCC 13 front end) | Yes, MSVC shipped it early (VS 2022 17.4+, even under `/std:c++20`) (unverified here) |
| `consteval` | Yes (C++20, verified via `typeHash` test) | Yes (C++20) | Yes (C++20) | Yes, VS 2019 16.8+ (unverified) |
| `static operator()` (C++23) | Untested this spike (not exercised: no gcc-14 available) | Presumed yes (full C++23 support) | Untested (same GCC 13 front end as deducing this — high risk of the same gap) | Yes, VS 2022 17.5+ (unverified) |
| `std::expected` (C++23 `<expected>`) | libstdc++ 13 header exists; not exercised (no fit found, see above) | libc++ 18 has it | libstdc++ for arm-none-eabi mirrors host libstdc++ version; not exercised | VS 2022 17.5+ (unverified) |

**Embedded toolchain implication:** the project's Cortex-M33 target (nRF54 application core) builds with
**arm-none-eabi-gcc, currently pinned to the 13.x line** in this environment. That is the same GCC 13 front
end that rejects deducing this. Two further data points, both **explicitly marked uncertain** (not verified
in this sandbox, from general knowledge that may be stale):
- **Zephyr SDK** (the toolchain nRF Connect SDK vendors) has historically bundled its own `arm-zephyr-eabi`
  GCC, typically one to two major versions behind mainline GCC (GCC 12.x in recent Zephyr SDK 0.16.x
  releases, to the best of available knowledge) — i.e. **likely further behind, not closer**, to GCC 14's
  deducing-this support than the arm-none-eabi-gcc 13.2 tested here.
- **nRF Connect SDK** tracks Zephyr's toolchain choice per release; no specific version was verified here.

If Sub0Pub ships to Zephyr-based nRF54 targets, **do not assume a future bump to arm-none-eabi-gcc 14 solves
this** — the actual embedded toolchain may be pinned independently and lag further. This should be confirmed
against the target SDK's documented toolchain version before any deducing-this-dependent code is required
(not merely offered) anywhere on the static/hot path.

## Recommendation

**Stay C++17 as the required minimum. Adopt C++20 as an optional, low-risk upgrade path. Treat C++23 as
enhancement-only, behind feature detection, never required.**

1. **C++20 concepts: recommend adopting, gated behind `__cpp_concepts`/`SUB0PUB_HAVE_CONCEPTS`, not a hard
   C++20 requirement.** Zero measured codegen cost on every toolchain tested (gcc, clang, arm-none-eabi-gcc,
   both forms, with and without LTO — see face-off 2), a real diagnostics improvement for a design that
   deliberately uses detection idioms as its central mechanism (`accepts`/`has_filter` in `sub0x_static.hpp`,
   the #8 broker's member-alias/ADL/traits resolution), and no toolchain gap among the three compilers this
   spike could test. This is the one change in this spike with a positive cost/benefit on every axis
   measured. It does not, by itself, justify raising the *required* language version, since the SFINAE
   fallback costs nothing and must be kept for C++17 users regardless.

2. **C++23 deducing this: do not require it.** It measurably matches the C++17 CRTP mixin's zero cost on
   Clang (face-off 1), but **GCC 13 and arm-none-eabi-gcc 13 reject it outright**, and the embedded target
   this project cares about (Cortex-M33 / nRF54, `docs/design/COLLAPSE_EVIDENCE.md`) is exactly the GCC 13
   toolchain that fails. It also does not solve the actual open question from `publisher_ergonomics.md`
   (whether a non-template publisher can be zero-cost) — the answer stays no, deducing this or not. If GCC
   14+ becomes available on every target this project ships to (embedded included, and Zephyr/nRF Connect
   SDK's own toolchain pin, not just mainline GCC), it is safe to *offer* `Publisher23<Out>` as sugar for the
   CRTP mixin, guarded by `__cpp_explicit_this_parameter`, with the C++17 mixin
   kept as the unconditional fallback. Never make it required.

3. **`consteval` for `typeHash`: worth doing under a C++20 (or higher) gate, independent of the rest.** It
   is a small, self-contained, zero-API-visible change (`utility::typeHash` is already private/internal) that
   converts a runtime-possible compile-time computation into a runtime-impossible one, at every optimisation
   level including `-O0`/debug builds — directly in the spirit of "correctness without cost" for a function
   whose entire job is to be free. Ship it behind the same C++20 feature gate as concepts.

4. **`static operator()`, `if consteval`, `std::expected`, `std::atomic::wait/notify`: no action now.** None
   has a codebase location where it currently helps (face-off 3); track them as candidates for the runtime
   registry / dynamic-subscription work `COLLAPSE_EVIDENCE.md` still has open, not for this issue's static
   collapse path.

**Bottom line for the "auto semantics" the maintainer asked about:** the C++20/23 features that touch actual
correctness-and-cost work (concepts, `consteval`) are gated, low-risk, zero-cost wins available *without*
raising the minimum standard, because they degrade gracefully to the existing C++17 SFINAE/`constexpr` code
on toolchains that lack them. The one feature that changes ergonomics in a way this issue cares about
(deducing this, for the publisher mixin) is exactly the one the embedded toolchain cannot build. **Raising
the *required* minimum to C++20 or C++23 is not justified by this evidence** — v2's collapse-critical path
(`StaticWiring`, pattern B2) is already proven zero-cost on C++17 (`COLLAPSE_EVIDENCE.md` Phase 1), and
nothing measured here changes that. The right shape is: **C++17 required, C++20 features opt-in behind
feature detection, C++23 features offered only where the target toolchain proves it can build them.**

#!/usr/bin/env python3
"""Collapse score matrix (issue #9 / PR #10): scores every option of every design axis from a collapse_evidence.py run.

Input is the JSON written by `collapse_evidence.py --json`. Output (Markdown, stdout) is the score tables of
docs/design/COLLAPSE_SCORES.md; nothing in them is computed by hand.

Rubric (docs/design/AXIS_SCORES.md): per metric, the ratio of an option to the cheapest option of the same axis in
the same case (the axis's hand-written references included), geometric mean over the cases where the option
exists: 3 <= x1.10, 2 <= x1.35, 1 <= x2.0, 0 above. Metrics (observable-work form):
  gcc / clang publish     callgrind instructions per publication (gcc-O2, clang-O2)
  setup+teardown          callgrind instructions of one construction plus one destruction (gcc-O2)
  cm33 path               static instructions on the publish path of the final Cortex-M33 ELF (cm33-gcc-Os),
                          compared only among options making the same number of indirect calls (work behind an
                          indirect call is invisible to the path metric); the most indirect calls is shown
  cm33 text / RAM         application bytes: the image minus the zero_receivers/handwritten image of the same
                          build (driver and C runtime only), so the ratio is not diluted by the C runtime
  cm33 deps               link dependencies added over the case's `handwritten` (TLS, operator delete, ...)
Noise floors: a difference within 2 instructions, 16 B of text or 8 B of RAM counts as equal (ratio 1), and byte
denominators are floored at 64 B (text) and 16 B (RAM) so that a few bytes over a near-empty application do not
dominate. Deps score: 3 none added, 1 one added, 0 two or more (worst case over the option's cases).
The last column checks each option against the equal-work reference it declares (`// SUB0X_REFERENCE:`), with the
collapse_evidence.py criteria, over every build and both forms: "=" means it meets every criterion everywhere.

Usage: python3 tests/collapse/collapse_scores.py RUN.json > scores.md
"""
import json
import math
import os
import sys
from collections import OrderedDict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import collapse_evidence as ev  # noqa: E402  (variant_reference, verdicts)

TOL = {"instr": 2.0, "text": 16, "ram": 8}
FLOOR = {"instr": 1.0, "text": 64, "ram": 16}

# Axis: (title, cases, options). An option is (label, {case: variant} or [candidate variant names], build suffix).
REG8 = ["sub0x_dynamic", "sub0x_dynamic_domain", "sub0x_dynamic_route"]
REG8_LEAN = [v + "_lean" for v in REG8]
BINDING_CASES = ["zero_receivers", "one_receiver", "multi_receivers", "filters", "many_receivers", "multi_types",
                 "large_payload", "nested_publish", "two_domains", "transport_endpoint", "cross_file",
                 "dynamic_subscriptions"]
AXES = [
    ("Binding form", BINDING_CASES, [
        ("hand-written, static (`handwritten`)", ["handwritten"], ""),
        ("hand-written, runtime addresses (`handwritten_runtime`)", ["handwritten_runtime"], ""),
        ("hand-written, context + function pointer (`handwritten_erased`)", ["handwritten_erased"], ""),
        ("B2 `StaticWiring`", ["sub0x_b2_static"], ""),
        ("B1 `wire(...)`", ["sub0x_b1_wire"], ""),
        ("B3 `Sink<T>`", ["sub0x_b3_sink"], ""),
        ("A today's API, default macros", ["sub0pub_virtual"], ""),
        ("A today's API, leanest macros", ["sub0pub_virtual_lean"], ""),
        ("#8 registry, default configuration", REG8, ""),
        ("#8 registry, leanest valid configuration", REG8_LEAN, ""),
    ]),
    ("Publisher spelling", ["publisher_ergonomics"], [
        ("hand-written, static", ["handwritten"], ""),
        ("hand-written, runtime addresses", ["handwritten_runtime"], ""),
        ("hand-written, context + function pointer", ["handwritten_erased"], ""),
        ("names the `StaticWiring` alias (alt6)", ["alt6_static_bound"], ""),
        ("`template<class Out>` by hand (alt1)", ["alt1_baseline_template"], ""),
        ("CRTP mixin `Publisher<Derived, Out>` (alt2)", ["alt2_crtp_mixin"], ""),
        ("CTAD factory (alt3)", ["alt3_ctad_factory"], ""),
        ("call-site output argument (alt4)", ["alt4_call_site_out"], ""),
        ("`Sink<T>` (alt5)", ["alt5_sink_typeerased"], ""),
        ("deducing-this mixin, C++23 (alt7)", ["alt7_deducing_this_mixin"], ""),
        ("deducing-this call site, C++23 (alt8)", ["alt8_deducing_this_callsite"], ""),
    ]),
    ("Static-path cancellation", ["cancellation", "cancellation_filtered"], [
        ("hand-written, static", ["handwritten"], ""),
        ("hand-written, runtime addresses", ["handwritten_runtime"], ""),
        ("`bool receive()` + `publishCancelable` (B2)", ["sub0x_alt1_bool"], ""),
        ("`bool receive()` + `publishCancelable` (B1, new)", ["sub0x_alt1_bool_b1"], ""),
        ("`std::expected` return, C++23 (B2)", ["sub0x_alt1c_expected_cpp23"], ""),
        ("`Delivery&` token (B2)", ["sub0x_alt2_token"], ""),
        ("`cancel()` flag, static storage (B2)", ["sub0x_alt3_static"], ""),
        ("`cancel()` flag, thread-local (B2)", ["sub0x_alt3_tls"], ""),
        ("filter on a shared flag (B2, no cancel support)", ["sub0x_alt4_filter"], ""),
    ]),
    ("Static/dynamic bridge", ["static_dynamic_bridge", "static_dynamic_bridge_empty", "static_dynamic_bridge_churn"], [
        ("hand-written, static + registry", ["handwritten"], ""),
        ("hand-written registry, empty (`handwritten_registry`)", ["handwritten_registry"], ""),
        ("`DynamicPort<T, N>`", ["sub0x_bridge_slots"], ""),
        ("`DynamicPort<T, N>`, C++23 bind result", ["sub0x_bridge_slots_cpp23"], ""),
        ("`BrokerPort<T>` to the #8 registry (lean)", ["sub0x_bridge_broker"], ""),
        ("inverted: static wiring as one registry subscriber", ["sub0x_bridge_inverted"], ""),
    ]),
    ("Transport endpoint and split horizon", ["transport_endpoint", "transport_two_links"], [
        ("hand-written, static", ["handwritten"], ""),
        ("hand-written, runtime addresses", ["handwritten_runtime"], ""),
        ("hand-written, context + function pointer", ["handwritten_erased"], ""),
        ("B2 `StaticForward`, origin = the binding", ["sub0x_b2_static"], ""),
        ("B2 `StaticForward`, origin = the transport (new)", ["sub0x_b2_static_origin_transport"], ""),
        ("B1 `Forward`, origin by type", {"transport_endpoint": "sub0x_b1_wire",
                                          "transport_two_links": "sub0x_b1_wire_typed_links"}, ""),
        ("B1 `Forward`, origin = the transport object", {"transport_endpoint": "sub0x_b1_wire_origin_transport",
                                                         "transport_two_links": "sub0x_b1_wire"}, ""),
        ("B3 `Sink<T>` + `Forward`", ["sub0x_b3_sink"], ""),
        ("#8 `Route`, default configuration", ["sub0x_dynamic_route"], ""),
        ("#8 `Route`, leanest valid configuration", ["sub0x_dynamic_route_lean"], ""),
    ]),
    ("Domains and sessions", ["two_domains"], [
        ("hand-written, static", ["handwritten"], ""),
        ("hand-written, runtime addresses", ["handwritten_runtime"], ""),
        ("hand-written, context + function pointer", ["handwritten_erased"], ""),
        ("hand-written, one gateway holding both domains' addresses (new)", ["handwritten_gateway"], ""),
        ("B2: one `StaticWiring` per domain", ["sub0x_b2_static"], ""),
        ("B2: one publisher naming both wirings (new)", ["sub0x_b2_one_publisher"], ""),
        ("B1: one `wire(...)` per domain", ["sub0x_b1_wire"], ""),
        ("B1: one publisher holding both wirings (new)", ["sub0x_b1_one_publisher"], ""),
        ("B3: one `Sink<T>` per domain (new)", ["sub0x_b3_sink"], ""),
        ("#8 `Domain`, default configuration", ["sub0x_dynamic_domain"], ""),
        ("#8 `Domain`, leanest valid configuration", ["sub0x_dynamic_domain_lean"], ""),
    ]),
    ("Cross-TU and LTO", ["cross_file"], [
        ("hand-written, static, no LTO", ["handwritten"], ""),
        ("hand-written, static, LTO", ["handwritten"], "-lto"),
        ("hand-written, runtime addresses, no LTO", ["handwritten_runtime"], ""),
        ("hand-written, runtime addresses, LTO", ["handwritten_runtime"], "-lto"),
        ("hand-written, context + function pointer, no LTO (new)", ["handwritten_erased"], ""),
        ("hand-written, context + function pointer, LTO (new)", ["handwritten_erased"], "-lto"),
        ("B2, no LTO", ["sub0x_b2_static"], ""),
        ("B2, LTO", ["sub0x_b2_static"], "-lto"),
        ("B1, no LTO", ["sub0x_b1_wire"], ""),
        ("B1, LTO", ["sub0x_b1_wire"], "-lto"),
        ("B3, no LTO (new)", ["sub0x_b3_sink"], ""),
        ("B3, LTO (new)", ["sub0x_b3_sink"], "-lto"),
        ("A default, no LTO", ["sub0pub_virtual"], ""),
        ("A default, LTO", ["sub0pub_virtual"], "-lto"),
        ("A leanest, no LTO", ["sub0pub_virtual_lean"], ""),
        ("A leanest, LTO", ["sub0pub_virtual_lean"], "-lto"),
    ]),
    ("C++ standard of the spelling", ["filters", "multi_receivers", "cancellation", "static_dynamic_bridge",
                                      "publisher_ergonomics"], [
        ("C++17 spelling", {"filters": "sub0x_b2_static", "multi_receivers": "sub0x_b2_static",
                            "cancellation": "sub0x_alt1_bool", "static_dynamic_bridge": "sub0x_bridge_slots",
                            "publisher_ergonomics": "alt2_crtp_mixin"}, ""),
        ("C++20/23 spelling (concepts, `std::expected`, deducing this)",
         {"filters": "sub0x_b2_static_cxx20", "multi_receivers": "sub0x_b2_static_cxx20",
          "cancellation": "sub0x_alt1c_expected_cpp23", "static_dynamic_bridge": "sub0x_bridge_slots_cpp23",
          "publisher_ergonomics": "alt7_deducing_this_mixin"}, ""),
    ]),
]

BUILD_GCC, BUILD_CLANG, BUILD_CM33 = "gcc-O2", "clang-O2", "cm33-gcc-Os"


def load(path):
    with open(path) as fh:
        rows = json.load(fh)
    return {(r["build"], r["case"], r["form"], r["variant"]): r for r in rows}


def resolve(option, case, data):
    """The variant implementing an option in a case, or None."""
    _, names, suffix = option
    if isinstance(names, dict):
        names = [names[case]] if case in names else []
    for n in names:
        for b in (BUILD_GCC, BUILD_CLANG, BUILD_CM33):
            r = data.get((b + suffix, case, "observable", n))
            if r and "error" not in r and "skipped" not in r:
                return n
    return None


def metrics(data, case, variant, suffix):
    """Observable-form metrics of one variant, or None where not measured."""
    m = {}
    g = data.get((BUILD_GCC + suffix, case, "observable", variant))
    c = data.get((BUILD_CLANG + suffix, case, "observable", variant))
    a = data.get((BUILD_CM33 + suffix, case, "observable", variant))
    if g and "instr" in g and "skipped" not in g:
        m["gcc"] = g["instr"]["publish"]
        m["life"] = g["instr"]["setup"] + g["instr"]["teardown"]
    if c and "instr" in c:
        m["clang"] = c["instr"]["publish"]
    if a and "error" not in a and "skipped" not in a:
        floor = data.get((BUILD_CM33, "zero_receivers", "observable", "handwritten"))
        m["path"] = a["path"]["instructions"]
        m["indirect"] = a["path"]["indirect_calls"]
        m["text"] = a["sections"]["text"] - (floor["sections"]["text"] if floor else 0)
        ram = a["sections"]["data"] + a["sections"]["bss"]
        m["ram"] = ram - ((floor["sections"]["data"] + floor["sections"]["bss"]) if floor else 0)
        ref = data.get((BUILD_CM33 + suffix, case, "observable", "handwritten"))
        m["deps"] = [d for d in a["dependencies"] if not ref or d not in ref["dependencies"]]
    return m


KIND = {"gcc": "instr", "clang": "instr", "life": "instr", "path": "instr", "text": "text", "ram": "ram"}


def ratio(value, best, kind):
    if value - best <= TOL[kind]:
        return 1.0
    return value / max(best, FLOOR[kind])


def score(r):
    if r is None:
        return None
    return 3 if r <= 1.10 else 2 if r <= 1.35 else 1 if r <= 2.0 else 0


def geomean(xs):
    return math.exp(sum(math.log(x) for x in xs) / len(xs)) if xs else None


def reference_check(data, case, variant, suffix):
    """(passes, total, failed criteria) of a variant against its declared reference, over builds and forms."""
    ok = total = 0
    failed = OrderedDict()
    ref_name = ev.variant_reference(case, variant)
    if ref_name is None:
        return None
    for (b, c, f, v), r in data.items():
        if c != case or v != variant or "error" in r or "skipped" in r:
            continue
        if suffix and not b.endswith(suffix) or not suffix and b.endswith("-lto"):
            continue
        ref = data.get((b, c, f, ref_name))
        if not ref or "error" in ref or "skipped" in ref:
            continue
        verdict, _ = ev.verdicts(r, ref)
        total += 1
        bad = [k for k, good in verdict.items() if good is False]
        if not bad:
            ok += 1
        for k in bad:
            failed[k] = failed.get(k, 0) + 1
    return ok, total, failed, ref_name


def fmt_score(values):
    g = geomean(values)
    if g is None:
        return "-"
    return f"**{score(g)}** ×{g:.2f}"


def axis_tables(title, cases, options, data):
    out = [f"### {title}\n"]
    rows = []
    per_case = OrderedDict()
    present = {}
    for opt in options:
        present[opt[0]] = OrderedDict()
        for case in cases:
            v = resolve(opt, case, data)
            if v:
                present[opt[0]][case] = (v, metrics(data, case, v, opt[2]))
    # cheapest per case and metric, over every option of the axis present in that case
    best = {}
    for case in cases:
        for key in KIND:
            vals = [present[o[0]][case][1][key] for o in options if case in present[o[0]] and key in present[o[0]][case][1]]
            if vals:
                best[(case, key)] = min(vals)
        # The path metric follows direct calls only: work behind an indirect call is invisible to it. A path is
        # therefore compared only with paths that make the same number of indirect calls.
        for o in options:
            m = present[o[0]].get(case, (None, {}))[1]
            if "path" in m:
                k = (case, "path", m["indirect"])
                best[k] = min(best.get(k, m["path"]), m["path"])
    head = ["Option", "cases", "gcc publish", "clang publish", "setup+teardown", "cm33 path", "cm33 text", "cm33 RAM",
            "cm33 deps", "vs its equal-work reference"]
    out.append("| " + " | ".join(head) + " |")
    out.append("|" + "---|" * len(head))
    for opt in options:
        label = opt[0]
        cells = present[label]
        if not cells:
            out.append(f"| {label} | 0 | not built on this toolchain |" + " |" * (len(head) - 3))
            continue
        cols = [label, str(len(cells))]
        for key in ("gcc", "clang", "life", "path", "text", "ram"):
            if key == "path":
                rs = [ratio(m[key], best[(case, key, m["indirect"])], KIND[key]) for case, (_, m) in cells.items() if key in m]
                ind = max((m["indirect"] for _, m in cells.values() if "indirect" in m), default=0)
                cols.append(fmt_score(rs) + (f" ({ind} indirect)" if ind else ""))
                continue
            rs = [ratio(m[key], best[(case, key)], KIND[key]) for case, (_, m) in cells.items() if key in m]
            cols.append(fmt_score(rs))
        deps = OrderedDict()
        for case, (_, m) in cells.items():
            for d in m.get("deps", []):
                deps[d] = True
        worst = max((len(m.get("deps", [])) for _, m in cells.values()), default=0)
        dscore = 3 if worst == 0 else 1 if worst == 1 else 0
        cols.append(f"**{dscore}**" + (" " + ", ".join(deps) if deps else ""))
        checks = [reference_check(data, case, v, opt[2]) for case, (v, _) in cells.items()]
        checks = [c for c in checks if c]
        if not checks:
            cols.append("reference")
        else:
            ok = sum(c[0] for c in checks)
            tot = sum(c[1] for c in checks)
            refs = sorted({c[3] for c in checks})
            failed = OrderedDict()
            for c in checks:
                for k, n in c[2].items():
                    failed[k] = failed.get(k, 0) + n
            if ok == tot:
                cols.append(f"**=** ({tot}/{tot}; vs {', '.join('`' + r + '`' for r in refs)})")
            else:
                worst3 = ", ".join(f"{k} {n}" for k, n in sorted(failed.items(), key=lambda kv: -kv[1])[:3])
                cols.append(f"{ok}/{tot} (vs {', '.join('`' + r + '`' for r in refs)}; fails: {worst3})")
        out.append("| " + " | ".join(cols) + " |")
    out.append("")
    # Raw values per case, so every score can be traced to a measurement
    out.append(f"<details><summary>{title}: raw values per case (gcc publish / clang publish / setup+teardown / "
               f"cm33 path / cm33 app text / cm33 app RAM)</summary>\n")
    out.append("| Option | " + " | ".join(f"`{c}`" for c in cases) + " |")
    out.append("|" + "---|" * (len(cases) + 1))
    for opt in options:
        cells = present[opt[0]]
        if not cells:
            continue
        row = [opt[0]]
        for case in cases:
            if case not in cells:
                row.append("-")
                continue
            m = cells[case][1]
            def g(k, d=0):
                return f"{m[k]:.{d}f}" if k in m else "-"
            row.append(f"{g('gcc', 1)} / {g('clang', 1)} / {g('life')} / {g('path')} / {g('text')} / {g('ram')}")
        out.append("| " + " | ".join(row) + " |")
    out.append("\n</details>\n")
    return "\n".join(out)


def disagreements(data):
    """Variants whose verdict against their reference differs between gcc-O2 and clang-O2 (same form)."""
    out = []
    for (b, c, f, v), r in data.items():
        if b != BUILD_GCC or v.startswith("handwritten") or "error" in r or "skipped" in r:
            continue
        rc = data.get((BUILD_CLANG, c, f, v))
        ref_name = ev.variant_reference(c, v)
        rg, rcr = data.get((BUILD_GCC, c, f, ref_name)), data.get((BUILD_CLANG, c, f, ref_name))
        if not rc or not rg or not rcr or any(k in x for k in ("error", "skipped") for x in (rc, rg, rcr)):
            continue
        vg, _ = ev.verdicts(r, rg)
        vc, _ = ev.verdicts(rc, rcr)
        fg = sorted(k for k, ok in vg.items() if ok is False)
        fc = sorted(k for k, ok in vc.items() if ok is False)
        if (not fg) != (not fc):
            dg = r["instr"]["publish"] - rg["instr"]["publish"]
            dc = rc["instr"]["publish"] - rcr["instr"]["publish"]
            out.append(f"| `{c}` | `{v}` | {f} | {', '.join(fg) or 'PASS'} (publish {dg:+.1f}) | "
                       f"{', '.join(fc) or 'PASS'} (publish {dc:+.1f}) |")
    return out


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    data = load(sys.argv[1])
    print("<!-- generated by tests/collapse/collapse_scores.py from " + os.path.basename(sys.argv[1]) + " -->\n")
    for title, cases, options in AXES:
        print(axis_tables(title, cases, options, data))
    rows = disagreements(data)
    print("### gcc and clang disagreements\n")
    print("Variants whose verdict against their equal-work reference differs between gcc-O2 and clang-O2.\n")
    if rows:
        print("| Case | Variant | Form | gcc-O2 | clang-O2 |")
        print("|---|---|---|---|---|")
        print("\n".join(sorted(rows)))
    else:
        print("None.")
    print()
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Compare two collapse_evidence.py JSON runs: which published numbers and verdicts changed.

Used to record what a change to the evidence tool itself (parser, accounting, gating) did to earlier results
(docs/design/COLLAPSE_SCORES.md, "Defects found"). Only variants present in both runs are compared. Output is a
Markdown table of every changed path instruction count, call count, retained byte count or criterion verdict.

Usage: python3 tests/collapse/collapse_diff.py OLD.json NEW.json > diff.md
"""
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import collapse_evidence as ev  # noqa: E402


def load(path):
    with open(path) as fh:
        return {(r["build"], r["case"], r["form"], r["variant"]): r for r in json.load(fh)}


def ok(r):
    return r is not None and "error" not in r and "skipped" not in r


def failed(data, key):
    b, c, f, v = key
    r = data.get(key)
    ref = data.get((b, c, f, ev.variant_reference(c, v) or ev.REFERENCE))
    if not ok(r) or not ok(ref) or v == ev.REFERENCE:
        return None
    verdict, _ = ev.verdicts(r, ref)
    return sorted(k for k, good in verdict.items() if good is False)


def main():
    old, new = load(sys.argv[1]), load(sys.argv[2])
    rows = []
    for key in sorted(set(old) & set(new)):
        o, n = old[key], new[key]
        if not ok(o) or not ok(n):
            continue
        changes = []
        for label, get in (("path instr", lambda r: r["path"]["instructions"]),
                           ("calls d/i", lambda r: f"{r['path']['direct_calls']}/{r['path']['indirect_calls']}"),
                           ("retained B", lambda r: r["retained_sub0_bytes"])):
            if get(o) != get(n):
                changes.append(f"{label} {get(o)} → {get(n)}")
        fo, fn = failed(old, key), failed(new, key)
        if fo != fn:
            changes.append(f"verdict {'PASS' if not fo else 'FAIL ' + ', '.join(fo)} → "
                           f"{'PASS' if not fn else 'FAIL ' + ', '.join(fn)}")
        if changes:
            rows.append(f"| {key[0]} | `{key[1]}` | {key[2]} | `{key[3]}` | " + "; ".join(changes) + " |")
    print("| Build | Case | Form | Variant | Change (old → new) |")
    print("|---|---|---|---|---|")
    print("\n".join(rows) if rows else "| - | - | - | - | none |")
    return 0


if __name__ == "__main__":
    sys.exit(main())

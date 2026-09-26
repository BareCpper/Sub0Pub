#!/usr/bin/env python3
"""Collapse evidence tool (issue #9): final-link evidence that a Sub0Pub coding pattern compiles away.

For every case in tests/collapse/cases/<case>/ and every variant file in it, in both forms
(observable-work, removable-work), on every available named build, this links a real executable
(driver.cpp + variant) and records:

  runtime (host builds, run natively and under callgrind):
    checksum                    must equal handwritten's (equal observable behaviour)
    setup / publish / teardown  instructions (publish is per publication)
  final ELF (all builds):
    publish path                instructions of collapse_publish plus every function reachable from it by
                                direct calls, direct/indirect call counts, and the callees retained
    text / data / bss           of the whole image; deltas against handwritten are reported
    retained Sub0Pub            code/state symbols from namespace sub0 left in the image
    dependencies                TLS, operator delete, __cxa_pure_virtual, static initialisers
  and a verdict per criterion against handwritten (the equal-work reference) of the same build and form.

Usage:
  python3 tests/collapse/collapse_evidence.py [--case NAME] [--build NAME] [--json OUT.json] > report.md
Exit status is non-zero if any variant's checksum differs from handwritten (behaviour broken). Criterion
verdicts are reported, not enforced: pattern A (today's API) is expected to fail them; that is the gap.
"""
import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from collections import OrderedDict

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
INCLUDE = os.path.join(ROOT, "include")
CASES_DIR = os.path.join(HERE, "cases")
PUBLISHES = 1000  # driver.cpp kPublishes
REFERENCE = "handwritten"

PROTOTYPE = os.path.join(ROOT, "tests", "design", "broker_config")  # #8 runtime-registry prototype (dynamic variants)
COMMON = ["-std=c++17", "-DNDEBUG", "-ffunction-sections", "-fdata-sections", "-I" + HERE, "-I" + INCLUDE, "-I" + PROTOTYPE]

# Named builds. host builds run (checksum + callgrind); cross builds are analysed from the final ELF only.
BUILDS = OrderedDict([
    ("gcc-O2", {"cxx": "g++", "flags": ["-O2"], "ldflags": ["-Wl,--gc-sections", "-Wl,-z,now"],
                "objdump": "objdump", "nm": "nm", "size": "size", "run": True, "arch": "x86"}),
    ("clang-O2", {"cxx": "clang++", "flags": ["-O2"], "ldflags": ["-Wl,--gc-sections", "-Wl,-z,now"],
                  "objdump": "objdump", "nm": "nm", "size": "size", "run": True, "arch": "x86"}),
    ("cm33-gcc-Os", {"cxx": "arm-none-eabi-g++",
                     "flags": ["-Os", "-mcpu=cortex-m33", "-mthumb", "-mfloat-abi=hard", "-mfpu=fpv5-sp-d16",
                               "-fno-exceptions", "-fno-rtti", "-DCOLLAPSE_NO_STDIO"],
                     "ldflags": ["--specs=nano.specs", "--specs=nosys.specs", "-Wl,--gc-sections"],
                     "support": ["support/bare_metal_tls.cpp"],
                     "objdump": "arm-none-eabi-objdump", "nm": "arm-none-eabi-nm", "size": "arm-none-eabi-size",
                     "run": False, "arch": "arm"}),
])

# LTO counterparts: only meaningful (and only run) for cases whose variants span several translation units
for _name in list(BUILDS):
    _cfg = dict(BUILDS[_name])
    _cfg["flags"] = _cfg["flags"] + ["-flto"]
    _cfg["ldflags"] = _cfg["ldflags"] + ["-flto"] + [f for f in _cfg["flags"] if f.startswith("-O")]
    _cfg["multi_tu_only"] = True
    BUILDS[_name + "-lto"] = _cfg

FORMS = OrderedDict([("observable", 1), ("removable", 0)])

# Tolerances for "equivalent to handwritten" verdicts
INSTR_TOLERANCE = 2          # extra instructions per publication
PATH_TOLERANCE = 2           # extra static instructions on the publish path

DEPENDENCY_MARKERS = OrderedDict([
    ("TLS", re.compile(r"^(__aeabi_read_tp|__tls_get_addr)$")),
    ("operator delete", re.compile(r"^_ZdlPv")),
    ("pure virtual", re.compile(r"^__cxa_pure_virtual$")),
    ("atexit", re.compile(r"^(__cxa_atexit|__aeabi_atexit|atexit)$")),
])


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def available_builds():
    return OrderedDict((n, b) for n, b in BUILDS.items() if shutil.which(b["cxx"]) and shutil.which(b["objdump"]))


def discover_cases(only=None):
    cases = OrderedDict()
    for name in sorted(os.listdir(CASES_DIR)):
        d = os.path.join(CASES_DIR, name)
        if not os.path.isdir(d) or (only and name != only):
            continue
        # A variant is a single file (<variant>.cpp) or a directory of translation units (<variant>/*.cpp)
        variants = sorted(f[:-4] for f in os.listdir(d) if f.endswith(".cpp"))
        variants += sorted(f for f in os.listdir(d) if os.path.isdir(os.path.join(d, f)))
        if REFERENCE not in variants:
            sys.exit(f"case {name}: missing {REFERENCE}")
        # Extra equal-work references (handwritten_<kind>, e.g. handwritten_runtime) are listed first; each is
        # itself compared with `handwritten`, and a variant selects one with `// SUB0X_REFERENCE: <name>`
        refs = [REFERENCE] + [v for v in variants if v.startswith(REFERENCE + "_")]
        cases[name] = refs + [v for v in variants if v not in refs]
    return cases


def variant_sources(case, variant):
    path = os.path.join(CASES_DIR, case, variant)
    if os.path.isdir(path):
        return sorted(os.path.join(path, f) for f in os.listdir(path) if f.endswith(".cpp"))
    return [path + ".cpp"]


STD_MARKER = re.compile(r"^//\s*SUB0X_STD:\s*(c\+\+\d+)\s*$")


def variant_std(sources):
    """A variant opts into a non-default -std= by making its first line `// SUB0X_STD: c++23`.
    All C++17 variants are untouched; this only affects variants that ask for it explicitly."""
    for src in sources:
        try:
            with open(src) as fh:
                first = fh.readline()
        except OSError:
            continue
        m = STD_MARKER.match(first.strip())
        if m:
            return m.group(1)
    return None


REF_MARKER = re.compile(r"^//\s*SUB0X_REFERENCE:\s*(\w+)\s*$")


def variant_reference(case, variant):
    """The equal-work reference a variant is judged against: `handwritten` unless one of its sources' leading
    comment lines names another reference of the same case (`// SUB0X_REFERENCE: handwritten_runtime`)."""
    if variant == REFERENCE:
        return None
    for src in variant_sources(case, variant):
        try:
            with open(src) as fh:
                head = [fh.readline() for _ in range(4)]
        except OSError:
            continue
        for line in head:
            m = REF_MARKER.match(line.strip())
            if m:
                return m.group(1)
    return REFERENCE


def multi_tu(case, variants):
    return any(os.path.isdir(os.path.join(CASES_DIR, case, v)) for v in variants)


def build(build_cfg, case, variant, observable, out_dir):
    exe = os.path.join(out_dir, f"{case}-{variant}-{observable}.elf")
    mapfile = exe[:-4] + ".map"
    sources = variant_sources(case, variant)
    std = variant_std(sources)
    common = COMMON if std is None else [f"-std={std}" if f.startswith("-std=") else f for f in COMMON]
    cmd = [build_cfg["cxx"], *common, *build_cfg["flags"], f"-DCOLLAPSE_OBSERVABLE={observable}",
           os.path.join(HERE, "driver.cpp"), *sources,
           *(os.path.join(HERE, f) for f in build_cfg.get("support", [])),
           "-o", exe, *build_cfg["ldflags"], f"-Wl,-Map={mapfile}"]
    r = run(cmd)
    if r.returncode != 0:
        lines = r.stderr.splitlines()
        err = next((l.split(": ", 1)[-1] for l in lines if "undefined reference" in l),
                   next((l for l in lines if "error" in l), r.stderr.strip()[:200]))
        return None, err
    return exe, None


def checksum(exe):
    r = run([exe])
    m = re.search(r"checksum (.*)", r.stdout)
    return m.group(1).strip() if m else f"exit={r.returncode}"


def callgrind(exe):
    phases = {}
    with tempfile.TemporaryDirectory() as tmp:
        run(["valgrind", "--tool=callgrind", "--callgrind-out-file=" + os.path.join(tmp, "cg.%p"), exe])
        for f in os.listdir(tmp):
            label, total = None, None
            with open(os.path.join(tmp, f)) as fh:
                for line in fh:
                    if line.startswith("desc: Trigger: Client Request: "):
                        label = line.split("Client Request: ", 1)[1].strip()
                    elif line.startswith("summary:"):
                        total = int(line.split()[1])
            if label and total is not None:
                phases[label] = total
    if "publish" in phases:
        phases["publish"] = phases["publish"] / PUBLISHES
    return phases


def disassemble(build_cfg, exe):
    """Return {function: [(mnemonic, operands)]} for every function symbol in the image."""
    out = run([build_cfg["objdump"], "-d", "--no-show-raw-insn", "-C", exe]).stdout
    funcs, current = {}, None
    for line in out.splitlines():
        m = re.match(r"^[0-9a-f]+ <(.+)>:$", line)
        if m:
            current = m.group(1)
            funcs[current] = []
            continue
        m = re.match(r"^\s+[0-9a-f]+:\s+(\S+)\s*(.*)$", line)
        if m and current is not None:
            funcs[current].append((m.group(1), m.group(2)))
    return funcs


def is_call(arch, mnemonic):
    return (mnemonic.startswith("call") if arch == "x86" else mnemonic in ("bl", "blx"))


def is_tail_jump(arch, mnemonic, operands, funcs, current):
    target = re.search(r"<([^>+]+)>", operands)
    if not target or target.group(1) == current or target.group(1) not in funcs:
        return None
    if arch == "x86" and mnemonic.startswith("jmp"):
        return target.group(1)
    if arch == "arm" and mnemonic in ("b", "b.w", "b.n"):
        return target.group(1)
    return None


def publish_path(build_cfg, funcs):
    """Instructions and calls on the publication path: collapse_publish plus directly reachable functions."""
    arch = build_cfg["arch"]
    seen, stack = [], ["collapse_publish"]
    instructions = direct = indirect = 0
    external = set()
    while stack:
        f = stack.pop()
        if f in seen or f not in funcs:
            if f not in funcs:
                external.add(f)
            continue
        seen.append(f)
        for mnemonic, operands in funcs[f]:
            instructions += 1
            target = re.search(r"<([^>+]+)", operands)
            if is_call(arch, mnemonic):
                if target is None or "*" in operands or (arch == "arm" and mnemonic == "blx" and re.match(r"r\d", operands)):
                    indirect += 1
                else:
                    direct += 1
                    name = target.group(1).split("@")[0]
                    stack.append(name) if name in funcs else external.add(name)
            else:
                tail = is_tail_jump(arch, mnemonic, operands, funcs, f)
                if tail:
                    direct += 1
                    stack.append(tail)
                elif arch == "x86" and mnemonic.startswith("jmp") and "*" in operands:
                    indirect += 1
    return {"instructions": instructions, "direct_calls": direct, "indirect_calls": indirect,
            "functions": seen, "external": sorted(external)}


def sections(build_cfg, exe):
    out = run([build_cfg["size"], exe]).stdout.splitlines()[1].split()
    text, data, bss = (int(x) for x in out[:3])
    init = 0
    for line in run([build_cfg["size"], "-A", exe]).stdout.splitlines():
        parts = line.split()
        if parts and parts[0] == ".init_array":
            init = int(parts[1])
    return {"text": text, "data": data, "bss": bss, "init_array": init}


def symbols(build_cfg, exe):
    out = run([build_cfg["nm"], "-S", "--size-sort", exe]).stdout
    demangled = run([build_cfg["nm"], "-C", "-S", "--size-sort", exe]).stdout
    raw = [l.split()[-1] for l in out.splitlines() if l.split()]
    retained = []
    for line in demangled.splitlines():
        parts = line.split(None, 3)
        if len(parts) == 4 and "sub0::" in parts[3]:
            retained.append((int(parts[1], 16), parts[3]))
    sizes = {}
    for line in demangled.splitlines():
        parts = line.split(None, 3)
        if len(parts) == 4:
            sizes[parts[3]] = sizes.get(parts[3], 0) + int(parts[1], 16)
    undefined = run([build_cfg["nm"], "-u", exe]).stdout.split()
    names = set(raw) | set(undefined)
    deps = [label for label, rx in DEPENDENCY_MARKERS.items() if any(rx.match(n) for n in names)]
    return {"retained_sub0_bytes": sum(s for s, _ in retained), "retained_sub0": [n for _, n in retained],
            "dependencies": deps, "symbols": sizes}


def measure(build_name, build_cfg, case, variant, form, observable, out_dir):
    exe, err = build(build_cfg, case, variant, observable, out_dir)
    if exe is None:
        return {"error": err}
    result = {"exe": exe}
    funcs = disassemble(build_cfg, exe)
    result["path"] = publish_path(build_cfg, funcs)
    result["sections"] = sections(build_cfg, exe)
    result.update(symbols(build_cfg, exe))
    if build_cfg["run"]:
        result["checksum"] = checksum(exe)
        if shutil.which("valgrind"):
            result["instr"] = callgrind(exe)
    return result


def verdicts(result, ref):
    """Criteria against the equal-work reference: True = meets, False = fails, None = not measured."""
    v = OrderedDict()
    if "checksum" in result:
        v["same behaviour"] = result["checksum"] == ref.get("checksum")
    if "instr" in result and "instr" in ref:
        for phase in ("publish", "setup", "teardown"):
            if phase in result["instr"]:
                tol = INSTR_TOLERANCE if phase == "publish" else 0
                v[f"{phase} instr"] = result["instr"][phase] <= ref["instr"][phase] + tol
    v["publish path"] = result["path"]["instructions"] <= ref["path"]["instructions"] + PATH_TOLERANCE
    v["no extra indirect calls"] = result["path"]["indirect_calls"] <= ref["path"]["indirect_calls"]
    v["no extra RAM"] = (result["sections"]["data"] + result["sections"]["bss"]) <= (ref["sections"]["data"] + ref["sections"]["bss"])
    v["no static init"] = result["sections"]["init_array"] <= ref["sections"]["init_array"]
    v["no Sub0Pub retained"] = result["retained_sub0_bytes"] == 0
    extra = [d for d in result["dependencies"] if d not in ref["dependencies"]]
    v["no extra dependencies"] = not extra
    return v, extra


def fmt_delta(value, ref, digits=0):
    d = value - ref
    return f"{value:.{digits}f} ({d:+.{digits}f})"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--case")
    ap.add_argument("--build")
    ap.add_argument("--json")
    args = ap.parse_args()

    builds = available_builds()
    if args.build:
        builds = OrderedDict((n, b) for n, b in builds.items() if n == args.build)
    cases = discover_cases(args.case)

    results = OrderedDict()
    broken = []
    with tempfile.TemporaryDirectory() as tmp:
        for build_name, build_cfg in builds.items():
            for case, variants in cases.items():
                if build_cfg.get("multi_tu_only") and not multi_tu(case, variants):
                    continue
                for form, observable in FORMS.items():
                    for variant in variants:
                        key = (build_name, case, form, variant)
                        results[key] = measure(build_name, build_cfg, case, variant, form, observable, tmp)

    print("# Collapse evidence (issue #9)\n")
    print("Final-link evidence per case, build and form; every variant is compared with `handwritten` "
          "(equal-work reference, same build and form), or with the extra reference it names, shown as "
          "`variant (vs handwritten_<kind>)`: e.g. `handwritten_runtime`, hand-written code that reaches its "
          "receivers through addresses stored at setup. Deltas in parentheses. "
          f"instr = callgrind instructions (publish: per publication of {PUBLISHES}). "
          "path = static instructions of `collapse_publish` plus directly reachable functions.\n")
    for build_name, build_cfg in builds.items():
        ver = run([build_cfg["cxx"], "--version"]).stdout.splitlines()[0]
        print(f"- **{build_name}**: `{ver}` `{' '.join(build_cfg['flags'])}`")
    print()

    for case, variants in cases.items():
        print(f"## Case: {case}\n")
        for build_name, build_cfg in builds.items():
            if (build_name, case, "observable", REFERENCE) not in results:
                continue
            for form in FORMS:
                ref = results[(build_name, case, form, REFERENCE)]
                print(f"### {build_name}, {form} work\n")
                if "error" in ref:
                    print(f"handwritten failed to build: `{ref['error']}`\n")
                    continue
                cols = ["variant", "checksum", "publish instr", "setup instr", "teardown instr", "path instr",
                        "calls (direct/indirect)", "text", "data+bss", "retained sub0 (B)", "added deps", "verdict"]
                print("| " + " | ".join(cols) + " |")
                print("|" + "---|" * len(cols))
                for variant in variants:
                    r = results[(build_name, case, form, variant)]
                    if "error" in r:
                        print(f"| {variant} | build error: `{r['error']}` |" + " |" * (len(cols) - 2))
                        continue
                    ref_name = variant_reference(case, variant)
                    ref = results[(build_name, case, form, ref_name or REFERENCE)]
                    if "error" in ref:
                        print(f"| {variant} | reference {ref_name} failed to build |" + " |" * (len(cols) - 2))
                        continue
                    shown = variant if ref_name in (None, REFERENCE) else f"{variant} (vs {ref_name})"
                    v, extra = verdicts(r, ref) if variant != REFERENCE else (OrderedDict(), [])
                    if variant != REFERENCE and v.get("same behaviour") is False:
                        broken.append((build_name, case, form, variant))
                    ins = r.get("instr", {})
                    rins = ref.get("instr", {})
                    def phase(p, digits):
                        return fmt_delta(ins[p], rins[p], digits) if p in ins and p in rins else "-"
                    ram = r["sections"]["data"] + r["sections"]["bss"]
                    rram = ref["sections"]["data"] + ref["sections"]["bss"]
                    failed = [k for k, ok in v.items() if ok is False]
                    verdict = "reference" if variant == REFERENCE else ("PASS" if not failed else "FAIL: " + ", ".join(failed))
                    if variant.startswith(REFERENCE + "_"):
                        verdict = "reference; " + verdict
                    same = "-" if "checksum" not in r else ("ok" if variant == REFERENCE or r["checksum"] == ref.get("checksum") else "**DIFFERS**")
                    print("| " + " | ".join([
                        shown, same, phase("publish", 1), phase("setup", 0), phase("teardown", 0),
                        fmt_delta(r["path"]["instructions"], ref["path"]["instructions"]),
                        f"{r['path']['direct_calls']}/{r['path']['indirect_calls']}",
                        fmt_delta(r["sections"]["text"], ref["sections"]["text"]),
                        fmt_delta(ram, rram),
                        str(r["retained_sub0_bytes"]),
                        ", ".join(extra) or "-",
                        verdict]) + " |")
                print()
            # Where the extra image comes from: largest symbols a variant adds over the reference (observable form)
            for variant in variants[1:]:
                ref = results[(build_name, case, "observable", variant_reference(case, variant))]
                r = results[(build_name, case, "observable", variant)]
                if "error" in r or "error" in ref:
                    continue
                added = sorted(((sz, n) for n, sz in r["symbols"].items()
                                if n not in ref["symbols"] or sz > ref["symbols"][n]), reverse=True)[:8]
                if added:
                    print(f"<details><summary>{build_name}: largest symbols added by {variant} (bytes)</summary>\n")
                    for sz, n in added:
                        print(f"- {sz} `{n}`")
                    print("\n</details>\n")

    if args.json:
        serial = []
        for (b, c, f, v), r in results.items():
            r = {k: val for k, val in r.items() if k not in ("exe", "symbols")}
            serial.append({"build": b, "case": c, "form": f, "variant": v, **r})
        with open(args.json, "w") as fh:
            json.dump(serial, fh, indent=1)

    if broken:
        print("**Behaviour mismatch:** " + ", ".join("/".join(k) for k in broken))
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())

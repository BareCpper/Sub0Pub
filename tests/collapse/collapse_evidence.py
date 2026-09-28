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
    retained Sub0Pub            code/state symbols from namespaces sub0 and sub0x left in the image (reported
                                separately as sub0/sub0x)
    dependencies                TLS, operator delete, __cxa_pure_virtual, static initialisers
  and a verdict per criterion against handwritten (the equal-work reference) of the same build and form.

Usage:
  python3 tests/collapse/collapse_evidence.py [--case NAME] [--build NAME] [--json OUT.json] > report.md
  python3 tests/collapse/collapse_evidence.py --self-test     # check the publish-path parser
Exit status is non-zero if any variant's checksum differs from its reference (behaviour broken), if any build,
run or measurement fails, if a declared reference is missing, or if the case/build selection is empty. A variant
marked `// SUB0X_REQUIRES: <feature>` is skipped (reported, not failed) on a build whose compiler lacks the
feature (the same probes as tests/collapse/CMakeLists.txt). Cost criterion verdicts are reported, not enforced:
pattern A (today's API) is expected to fail them; that is the gap.
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


SUB0_NAMESPACE = re.compile(r"(^|[^\w])sub0x?::")
SUB0_ONLY = re.compile(r"(^|[^\w])sub0::")


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


REQUIRES_MARKER = re.compile(r"^//\s*SUB0X_REQUIRES:\s*(\S+)\s*$")

# Feature probes, equivalent to tests/collapse/CMakeLists.txt: a variant naming `// SUB0X_REQUIRES: <feature>`
# on its second line is skipped (reported, not failed) on a build whose compiler lacks the feature.
FEATURE_PROBES = {
    "deducing-this": ("struct S { template<class T> void f(this auto&& self, const T& t) noexcept "
                      "{ (void)self; (void)t; } };\nint main() { S s; s.f(1); return 0; }\n"),
    "expected": ("#include <expected>\n#if !defined(__cpp_lib_expected)\n#error no std::expected\n#endif\n"
                 "int main() { std::expected<void, int> e; return e ? 0 : 1; }\n"),
}
_probe_cache = {}


def variant_requires(sources):
    """The feature a variant needs (`// SUB0X_REQUIRES: <feature>` on the second line of its first source)."""
    try:
        with open(sources[0]) as fh:
            fh.readline()
            m = REQUIRES_MARKER.match(fh.readline().strip())
    except (OSError, IndexError):
        return None
    return m.group(1) if m else None


def has_feature(build_cfg, feature):
    key = (build_cfg["cxx"], tuple(build_cfg["flags"]), feature)
    if key not in _probe_cache:
        if feature not in FEATURE_PROBES:
            sys.exit(f"unknown SUB0X_REQUIRES feature: {feature}")
        with tempfile.TemporaryDirectory() as tmp:
            src = os.path.join(tmp, "probe.cpp")
            with open(src, "w") as fh:
                fh.write(FEATURE_PROBES[feature])
            flags = [f for f in build_cfg["flags"] if f != "-flto"]
            r = run([build_cfg["cxx"], "-std=c++23", *flags, "-fsyntax-only", src])
        _probe_cache[key] = r.returncode == 0
    return _probe_cache[key]


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
    """(checksum, error): the run must exit 0 and print its checksum."""
    r = run([exe])
    m = re.search(r"checksum (.*)", r.stdout)
    if r.returncode != 0 or not m:
        return None, f"run failed: exit={r.returncode} {r.stderr.strip()[:200]}"
    return m.group(1).strip(), None


def callgrind(exe):
    phases = {}
    with tempfile.TemporaryDirectory() as tmp:
        result = run(["valgrind", "--tool=callgrind", "--callgrind-out-file=" + os.path.join(tmp, "cg.%p"), exe])
        if result.returncode != 0:
            return phases, f"callgrind failed: exit={result.returncode} {result.stderr.strip()[:200]}"
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
    missing = [p for p in ("setup", "publish", "teardown") if p not in phases]
    return phases, (f"callgrind: missing phases {missing}" if missing else None)


def parse_objdump(text):
    """Parse `objdump -d --no-show-raw-insn -C` output.

    Returns (funcs, names): funcs maps each function's start address to [(mnemonic, operands)], names maps the
    start address to its demangled name. Functions are keyed by address, never by name: demangled names contain
    '<', '>', '+' and spaces, and two distinct functions can share a demangled name (anonymous namespaces in
    different TUs, constructor aliases)."""
    funcs, names, current = {}, {}, None
    for line in text.splitlines():
        m = re.match(r"^([0-9a-f]+) <(.+)>:$", line)
        if m:
            current = int(m.group(1), 16)
            funcs[current] = []
            names[current] = m.group(2)
            continue
        m = re.match(r"^\s+[0-9a-f]+:\s+(\S+)\s*(.*)$", line)
        if m and current is not None:
            funcs[current].append((m.group(1), m.group(2)))
    return funcs, names


def disassemble(build_cfg, exe):
    return parse_objdump(run([build_cfg["objdump"], "-d", "--no-show-raw-insn", "-C", exe]).stdout)


def is_call(arch, mnemonic):
    return (mnemonic.startswith("call") if arch == "x86" else mnemonic in ("bl", "blx"))


def branch_address(operands):
    """The numeric target of a direct branch (`call 401126 <ns::f<A<B>>(A<B> const&)>`), or None if indirect.

    The target is resolved from objdump's numeric address, not from the symbol text. The earlier parser cut the
    demangled name at the first '>' or '+', so a call to `sub0x::detail::deliver<R, M>(R&, M const&)` became an
    unresolvable name and its body was left off the publish path (scores review)."""
    m = re.match(r"^(?:0x)?([0-9a-f]+)(?:\s|$)", operands)
    return int(m.group(1), 16) if m else None


def is_plt(name):
    return "@plt" in name


def is_tail_jump(arch, mnemonic, operands, funcs, current):
    """A branch to the start of another function (a tail call). Branches inside a function are not calls."""
    if not ((arch == "x86" and mnemonic.startswith("jmp")) or (arch == "arm" and mnemonic in ("b", "b.w", "b.n"))):
        return None
    target = branch_address(operands)
    if target is None or target == current or target not in funcs:
        return None
    return target


def publish_path(build_cfg, funcs, names=None, root="collapse_publish"):
    """Instructions and calls on the publication path: collapse_publish plus directly reachable functions."""
    arch = build_cfg["arch"]
    if names is None:  # legacy shape {name: insns}
        names = {i: n for i, n in enumerate(funcs)}
        funcs = {i: funcs[n] for i, n in names.items()}
    start = next((a for a, n in names.items() if n == root), None)
    seen, stack = [], [start] if start is not None else []
    instructions = direct = indirect = 0
    external = set() if start is not None else {root}
    while stack:
        f = stack.pop()
        if f in seen:
            continue
        seen.append(f)
        for mnemonic, operands in funcs[f]:
            instructions += 1
            if is_call(arch, mnemonic):
                target = branch_address(operands)
                if target is None or "*" in operands or (arch == "arm" and mnemonic == "blx" and re.match(r"r\d", operands)):
                    indirect += 1
                else:
                    direct += 1
                    if target in funcs and not is_plt(names[target]):
                        stack.append(target)
                    else:
                        label = names.get(target) or (re.search(r"<(.*)>\s*$", operands) or [None, hex(target)])[1]
                        external.add(label.split("@")[0])
            else:
                tail = is_tail_jump(arch, mnemonic, operands, funcs, f)
                if tail is not None:
                    direct += 1
                    if is_plt(names[tail]):
                        external.add(names[tail].split("@")[0])
                    else:
                        stack.append(tail)
                elif arch == "x86" and mnemonic.startswith("jmp") and "*" in operands:
                    indirect += 1
    return {"instructions": instructions, "direct_calls": direct, "indirect_calls": indirect,
            "functions": [names[f] for f in seen], "external": sorted(external)}


SELF_TEST_X86 = """
0000000000401000 <collapse_publish>:
  401000:	push   %rbx
  401001:	call   401100 <void sub0x::detail::deliver<(anonymous namespace)::Controller, (anonymous namespace)::Sample>((anonymous namespace)::Controller&, (anonymous namespace)::Sample const&)>
  401006:	call   401200 <memcpy@plt>
  40100b:	call   *%rax
  40100d:	jne    401000 <collapse_publish>
  40100f:	pop    %rbx
  401010:	jmp    401300 <sub0x::Wiring<A<B<C> >, D>::publish<E>(E const&) const+0x0>

0000000000401100 <void sub0x::detail::deliver<(anonymous namespace)::Controller, (anonymous namespace)::Sample>((anonymous namespace)::Controller&, (anonymous namespace)::Sample const&)>:
  401100:	mov    (%rdi),%eax
  401102:	ret

0000000000401200 <memcpy@plt>:
  401200:	jmp    *0x2000(%rip)

0000000000401300 <sub0x::Wiring<A<B<C> >, D>::publish<E>(E const&) const>:
  401300:	add    $0x1,%eax
  401303:	jmp    401305 <sub0x::Wiring<A<B<C> >, D>::publish<E>(E const&) const+0x5>
  401305:	ret
"""

SELF_TEST_ARM = """
00008000 <collapse_publish>:
    8000:	push	{r4, lr}
    8002:	bl	8100 <sub0x::StaticWiring<&(anonymous namespace)::a, &(anonymous namespace)::b>::publish<S>(S const&)>
    8006:	blx	r3
    8008:	b.w	8200 <operator>>(A const&, B const&)>

00008100 <sub0x::StaticWiring<&(anonymous namespace)::a, &(anonymous namespace)::b>::publish<S>(S const&)>:
    8100:	bx	lr

00008200 <operator>>(A const&, B const&)>:
    8200:	movs	r0, #1
    8202:	bx	lr
"""


def self_test():
    """Checks the publish-path walk on synthetic objdump output: nested-template callees, a PLT stub, an
    indirect call, a tail call whose target name has an offset suffix, an intra-function branch, and (Arm) a
    tail call to an operator whose name contains '>'."""
    x86 = publish_path({"arch": "x86"}, *parse_objdump(SELF_TEST_X86))
    assert x86["instructions"] == 7 + 2 + 3, x86
    assert x86["direct_calls"] == 3 and x86["indirect_calls"] == 1, x86
    assert x86["external"] == ["memcpy"], x86
    assert any(f.startswith("void sub0x::detail::deliver<") for f in x86["functions"]), x86
    arm = publish_path({"arch": "arm"}, *parse_objdump(SELF_TEST_ARM))
    assert arm["instructions"] == 4 + 1 + 2 and arm["direct_calls"] == 2 and arm["indirect_calls"] == 1, arm
    assert "operator>>(A const&, B const&)" in arm["functions"], arm
    print("self-test passed")
    return 0


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
    retained, retained_x = [], []
    for line in demangled.splitlines():
        parts = line.split(None, 3)
        # Both namespaces, reported separately: sub0 (today's API) and sub0x (the #8 prototype and the pattern B
        # sandbox). Matching "sub0::" alone missed every sub0x symbol, so retained prototype and sandbox code was
        # invisible (scores review)
        if len(parts) == 4 and SUB0_NAMESPACE.search(parts[3]):
            (retained if SUB0_ONLY.search(parts[3]) else retained_x).append((int(parts[1], 16), parts[3]))
    sizes = {}
    for line in demangled.splitlines():
        parts = line.split(None, 3)
        if len(parts) == 4:
            sizes[parts[3]] = sizes.get(parts[3], 0) + int(parts[1], 16)
    undefined = run([build_cfg["nm"], "-u", exe]).stdout.split()
    names = set(raw) | set(undefined)
    deps = [label for label, rx in DEPENDENCY_MARKERS.items() if any(rx.match(n) for n in names)]
    return {"retained_sub0_bytes": sum(s for s, _ in retained) + sum(s for s, _ in retained_x),
            "retained_sub0_only_bytes": sum(s for s, _ in retained), "retained_sub0x_bytes": sum(s for s, _ in retained_x),
            "retained_sub0": [n for _, n in retained + retained_x],
            "dependencies": deps, "symbols": sizes}


def measure(build_name, build_cfg, case, variant, form, observable, out_dir):
    """A result dict; {"skipped": why} for an explicit unsupported-feature skip; {"error": why} for a failure."""
    needed = variant_requires(variant_sources(case, variant))
    if needed and not has_feature(build_cfg, needed):
        return {"skipped": f"{build_cfg['cxx']} lacks {needed}"}
    exe, err = build(build_cfg, case, variant, observable, out_dir)
    if exe is None:
        return {"error": err}
    result = {"exe": exe}
    funcs, names = disassemble(build_cfg, exe)
    result["path"] = publish_path(build_cfg, funcs, names)
    if "collapse_publish" in result["path"]["external"]:
        return {"error": "collapse_publish not found in the disassembly"}
    result["sections"] = sections(build_cfg, exe)
    result.update(symbols(build_cfg, exe))
    if build_cfg["run"]:
        result["checksum"], err = checksum(exe)
        if err:
            return {"error": err}
        if shutil.which("valgrind"):
            result["instr"], err = callgrind(exe)
            if err:
                return {"error": err}
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
    # Sub0Pub code (sub0:: or sub0x::) that survives as a named out-of-line function (a Sink thunk,
    # DynamicPort::receive) passes only if the image is no larger than the reference's, i.e. it is the same code
    # the reference has under another name. One rule for both namespaces: since Phase 2 the static wiring is
    # public (sub0::), and a runtime registry's retained code always makes its image larger
    v["no Sub0Pub retained"] = result["retained_sub0_bytes"] == 0 or result["sections"]["text"] <= ref["sections"]["text"]
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
    ap.add_argument("--self-test", action="store_true", help="check the publish-path parser and exit")
    args = ap.parse_args()
    if args.self_test:
        return self_test()

    builds = available_builds()
    if args.build:
        builds = OrderedDict((n, b) for n, b in builds.items() if n == args.build)
    if not builds:
        print(f"**No build selected:** {args.build or 'no toolchain available'}", file=sys.stderr)
        return 2
    cases = discover_cases(args.case)
    if not cases:
        print(f"**No case selected:** {args.case}", file=sys.stderr)
        return 2
    # A variant naming a reference the case does not have fails (it would silently fall back otherwise)
    failures = []
    for case, variants in cases.items():
        for variant in variants:
            ref_name = variant_reference(case, variant)
            if ref_name and ref_name not in variants:
                failures.append(f"{case}/{variant}: reference {ref_name} does not exist")

    results = OrderedDict()
    broken = []
    skipped = []
    with tempfile.TemporaryDirectory() as tmp:
        for build_name, build_cfg in builds.items():
            for case, variants in cases.items():
                if build_cfg.get("multi_tu_only") and not multi_tu(case, variants):
                    continue
                for form, observable in FORMS.items():
                    for variant in variants:
                        key = (build_name, case, form, variant)
                        results[key] = r = measure(build_name, build_cfg, case, variant, form, observable, tmp)
                        if "error" in r:
                            failures.append(f"{'/'.join(key)}: {r['error']}")
                        elif "skipped" in r:
                            skipped.append(f"{'/'.join(key)}: {r['skipped']}")
            ran = [k for k in results if k[0] == build_name]
            if ran and all("error" in results[k] or "skipped" in results[k] for k in ran):
                failures.append(f"{build_name}: nothing measured")
    if not results:
        failures.append("the case/build selection measured nothing (e.g. an LTO build with a single-TU case)")

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
                    print(f"handwritten failed: `{ref['error']}`\n")
                    continue
                cols = ["variant", "checksum", "publish instr", "setup instr", "teardown instr", "path instr",
                        "calls (direct/indirect)", "text", "data+bss", "retained sub0/sub0x (B)", "added deps", "verdict"]
                print("| " + " | ".join(cols) + " |")
                print("|" + "---|" * len(cols))
                for variant in variants:
                    r = results[(build_name, case, form, variant)]
                    if "error" in r:
                        print(f"| {variant} | **FAILED**: `{r['error']}` |" + " |" * (len(cols) - 2))
                        continue
                    if "skipped" in r:
                        print(f"| {variant} | skipped: {r['skipped']} |" + " |" * (len(cols) - 2))
                        continue
                    ref_name = variant_reference(case, variant)
                    ref = results.get((build_name, case, form, ref_name or REFERENCE), {"error": "missing"})
                    if "error" in ref or "skipped" in ref:
                        print(f"| {variant} | reference {ref_name} unavailable |" + " |" * (len(cols) - 2))
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
                        f"{r['retained_sub0_only_bytes']}/{r['retained_sub0x_bytes']}",
                        ", ".join(extra) or "-",
                        verdict]) + " |")
                print()
            # Where the extra image comes from: largest symbols a variant adds over the reference (observable form)
            for variant in variants[1:]:
                ref = results.get((build_name, case, "observable", variant_reference(case, variant)), {"error": "-"})
                r = results[(build_name, case, "observable", variant)]
                if any(k in x for k in ("error", "skipped") for x in (r, ref)):
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

    # Gating: behaviour mismatches, build/run/measurement failures and missing references fail the tool.
    # Explicit feature skips (SUB0X_REQUIRES) are listed, not failed. Cost verdicts stay report-only.
    if skipped:
        print("**Skipped (unsupported feature):** " + "; ".join(skipped) + "\n")
    status = 0
    if broken:
        print("**Behaviour mismatch:** " + ", ".join("/".join(k) for k in broken) + "\n")
        status = 1
    if failures:
        print("**Failed:** " + "; ".join(failures) + "\n")
        print(f"{len(failures)} failure(s)", file=sys.stderr)
        status = 1
    return status


if __name__ == "__main__":
    sys.exit(main())

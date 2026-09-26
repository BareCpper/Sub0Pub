#!/usr/bin/env bash
# Spike measurement driver for docs/design/spikes/embedded_size.md (issue #2).
# Links final ELFs for one_receiver / multi_receivers / zero_receivers against the today's-header
# variant (sub0pub_virtual.cpp) and the lever header (sub0pub_spike.cpp), sweeping config macros
# and structural levers, on cm33-gcc-Os. Prints a CSV-ish table: case,config,text,data,bss,deps.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
COLLAPSE="$ROOT/tests/collapse"
INCLUDE="$ROOT/include"
CXX=arm-none-eabi-g++
OUT="$ROOT/tests/footprint/spike/out"
mkdir -p "$OUT"

COMMON=(-std=c++17 -DNDEBUG -ffunction-sections -fdata-sections -I"$COLLAPSE" -I"$INCLUDE")
ARM_BASE=(-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -fno-rtti -DCOLLAPSE_NO_STDIO)
ARM_RTTI=(-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -DCOLLAPSE_NO_STDIO)
LDFLAGS=(--specs=nano.specs --specs=nosys.specs -Wl,--gc-sections)

build_one () {
  local case="$1" variant="$2" tag="$3"; shift 3
  local extra=("$@")
  local armflags=("${ARM_FLAGS[@]:-${ARM_BASE[@]}}")
  local src="$COLLAPSE/cases/$case/$variant.cpp"
  local elf="$OUT/${case}__${tag}.elf"
  if ! "$CXX" "${COMMON[@]}" "${armflags[@]}" "${extra[@]}" -c "$src" -o "$OUT/${case}__${tag}.o" 2>"$OUT/${case}__${tag}.err"; then
    echo "$case,$tag,BUILD-FAIL,,,$(tail -1 "$OUT/${case}__${tag}.err" | tr ',' ';')"
    return
  fi
  if ! "$CXX" "${armflags[@]}" "${LDFLAGS[@]}" "$OUT/${case}__${tag}.o" "$COLLAPSE/support/bare_metal_tls.cpp" "$COLLAPSE/driver.cpp" "${COMMON[@]}" "${extra[@]}" -o "$elf" 2>"$OUT/${case}__${tag}.linkerr"; then
    echo "$case,$tag,LINK-FAIL,,,$(tail -1 "$OUT/${case}__${tag}.linkerr" | tr ',' ';')"
    return
  fi
  local sz text data bss
  sz=$(arm-none-eabi-size "$elf" | tail -1)
  text=$(echo "$sz" | awk '{print $1}')
  data=$(echo "$sz" | awk '{print $2}')
  bss=$(echo "$sz" | awk '{print $3}')
  local deps=""
  local syms
  syms=$(arm-none-eabi-nm "$elf" 2>/dev/null || true)
  echo "$syms" | grep -qE '^\S+ [Tt] (__aeabi_read_tp|__tls_get_addr)$' && deps="${deps}TLS;"
  echo "$syms" | grep -qE '_ZdlPv' && deps="${deps}operator-delete;"
  echo "$syms" | grep -qE '__cxa_pure_virtual' && deps="${deps}pure-virtual;"
  echo "$syms" | grep -qE '(__cxa_atexit|__aeabi_atexit)' && deps="${deps}atexit;"
  echo "$syms" | grep -qE '_sbrk|malloc' && deps="${deps}malloc/sbrk;"
  [ -z "$deps" ] && deps="-"
  echo "$case,$tag,$text,$data,$bss,$deps"
}

echo "case,config,text,data,bss,deps"
for case in zero_receivers one_receiver multi_receivers; do
  # A: today's public header, default config (matches existing sub0pub_virtual cm33 build)
  build_one "$case" sub0pub_virtual A_today "-DSUB0PUB_TYPEIDNAME=0" "-DSUB0PUB_STD=0" "-DSUB0PUB_THREAD_SAFE=0"
  # B: today's header, RTTI enabled (drop -fno-rtti), as issue #2 describes
  ARM_FLAGS=("${ARM_RTTI[@]}")
  build_one "$case" sub0pub_virtual B_rtti_on "-DSUB0PUB_TYPEIDNAME=0" "-DSUB0PUB_STD=0" "-DSUB0PUB_THREAD_SAFE=0"
  unset ARM_FLAGS
  # C: today's header, SUB0PUB_TYPEIDNAME=1 (typeHash/typeName + diagnostic operator<<, which needs OStream => SUB0PUB_STD=1;
  #    TYPEIDNAME=1 with SUB0PUB_STD=0 fails to compile at all on this header, see report)
  build_one "$case" sub0pub_virtual C_typeidname "-DSUB0PUB_TYPEIDNAME=1" "-DSUB0PUB_STD=1" "-DSUB0PUB_THREAD_SAFE=0"
  # D: today's header, SUB0PUB_STD=1 (iostream path compiled, not necessarily called)
  build_one "$case" sub0pub_virtual D_std_iostream "-DSUB0PUB_TYPEIDNAME=0" "-DSUB0PUB_STD=1" "-DSUB0PUB_THREAD_SAFE=0"
  # E: spike header, non-virtual Subscribe dtor
  build_one "$case" sub0pub_spike E_sub_novdtor "-DSPIKE_SUB_VDTOR=0"
  # F: spike header, non-virtual Publish dtor (Publish<T> loses its vtable entirely)
  build_one "$case" sub0pub_spike F_pub_novdtor "-DSPIKE_PUB_VDTOR=0"
  # G: spike header, both non-virtual dtors
  build_one "$case" sub0pub_spike G_both_novdtor "-DSPIKE_SUB_VDTOR=0" "-DSPIKE_PUB_VDTOR=0"
  # H: spike header, both non-virtual dtors + non-virtual filter()
  build_one "$case" sub0pub_spike H_both_novdtor_nofilter "-DSPIKE_SUB_VDTOR=0" "-DSPIKE_PUB_VDTOR=0" "-DSPIKE_FILTER_VIRTUAL=0"
  # I: spike header, plain static instead of thread_local (single-threaded)
  build_one "$case" sub0pub_spike I_no_tls "-DSPIKE_TLS=0"
  # J: spike header, everything combined (G+H+I)
  build_one "$case" sub0pub_spike J_all_combined "-DSPIKE_SUB_VDTOR=0" "-DSPIKE_PUB_VDTOR=0" "-DSPIKE_FILTER_VIRTUAL=0" "-DSPIKE_TLS=0"
  # K: today's header, SUB0PUB_THREAD_SAFE=1 (std::mutex on bare metal)
  build_one "$case" sub0pub_virtual K_thread_safe "-DSUB0PUB_TYPEIDNAME=0" "-DSUB0PUB_STD=0" "-DSUB0PUB_THREAD_SAFE=1"
done

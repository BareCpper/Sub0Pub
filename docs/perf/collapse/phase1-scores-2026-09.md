# Collapse evidence (issue #9)

Final-link evidence per case, build and form; every variant is compared with `handwritten` (equal-work reference, same build and form), or with the extra reference it names, shown as `variant (vs handwritten_<kind>)`: e.g. `handwritten_runtime`, hand-written code that reaches its receivers through addresses stored at setup. Deltas in parentheses. instr = callgrind instructions (publish: per publication of 1000). path = static instructions of `collapse_publish` plus directly reachable functions.

- **gcc-O2**: `g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` `-O2`
- **clang-O2**: `Ubuntu clang version 18.1.3 (1ubuntu1)` `-O2`
- **cm33-gcc-Os**: `arm-none-eabi-g++ (15:13.2.rel1-2) 13.2.1 20231009` `-Os -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -fno-rtti -DCOLLAPSE_NO_STDIO`
- **gcc-O2-lto**: `g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` `-O2 -flto`
- **clang-O2-lto**: `Ubuntu clang version 18.1.3 (1ubuntu1)` `-O2 -flto`
- **cm33-gcc-Os-lto**: `arm-none-eabi-g++ (15:13.2.rel1-2) 13.2.1 20231009` `-Os -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -fno-rtti -DCOLLAPSE_NO_STDIO -flto`

## Case: cancellation

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 31.3 (+4.3) | 26 (+6) | 16 (+0) | 34 (+5) | 0/0 | 2495 (+64) | 656 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_alt1_bool | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | 30.7 (-0.7) | 26 (+0) | 16 (+0) | 34 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | 29.0 (+2.0) | 20 (+0) | 16 (+0) | 30 (+1) | 0/0 | 2431 (+0) | 632 (+8) | 0/1 | - | FAIL: no extra RAM |
| sub0x_alt3_tls | ok | 30.0 (+3.0) | 20 (+0) | 16 (+0) | 31 (+2) | 0/0 | 2447 (+16) | 625 (+1) | 0/1 | - | FAIL: publish instr, no extra RAM, no Sub0Pub retained |
| sub0x_alt4_filter | ok | 33.3 (+6.3) | 21 (+1) | 16 (+0) | 36 (+7) | 0/0 | 2463 (+32) | 632 (+8) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 12.3 (+0.7) | 26 (+6) | 16 (+0) | 10 (+1) | 0/0 | 2415 (+48) | 656 (+32) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_alt1_bool | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | 12.3 (+0.0) | 26 (+0) | 16 (+0) | 10 (+0) | 0/0 | 2415 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt4_filter | ok | 14.7 (+3.0) | 21 (+1) | 16 (+0) | 11 (+2) | 0/0 | 2367 (+0) | 632 (+8) | 0/0 | - | FAIL: publish instr, setup instr, no extra RAM |

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 114 `collapse_publish`
- 67 `collapse_setup`
- 24 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::gate`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_alt3_static (bytes)</summary>

- 111 `collapse_publish`
- 1 `sub0x::detail::g_canceled`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_alt3_tls (bytes)</summary>

- 124 `collapse_publish`
- 1 `sub0x::detail::g_canceled`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_alt4_filter (bytes)</summary>

- 130 `collapse_publish`
- 32 `collapse_setup`
- 1 `(anonymous namespace)::stopped`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0x_alt1_bool | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | skipped: clang++ lacks expected | | | | | | | | | | |
| sub0x_alt2_token | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt4_filter | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 11.7 (+0.0) | 18 (+1) | 14 (+0) | 8 (+0) | 0/0 | 2106 (+16) | 664 (+0) | 0/0 | - | reference; FAIL: setup instr |
| sub0x_alt1_bool | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | 11.7 (+0.0) | 18 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | skipped: clang++ lacks expected | | | | | | | | | | |
| sub0x_alt2_token | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt4_filter | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0/0 | - | reference |
| handwritten_runtime | - | - | - | - | 35 (+3) | 0/0 | 1204 (+20) | 532 (+16) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_alt1_bool | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | - | - | - | - | 35 (+0) | 0/0 | 1204 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | - | - | - | - | 37 (+5) | 0/0 | 1196 (+12) | 520 (+4) | 0/1 | - | FAIL: publish path, no extra RAM, no Sub0Pub retained |
| sub0x_alt3_tls | - | - | - | - | 46 (+14) | 2/0 | 1220 (+36) | 773 (+257) | 0/1 | TLS | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_alt4_filter | - | - | - | - | 43 (+11) | 0/0 | 1216 (+32) | 520 (+4) | 0/0 | - | FAIL: publish path, no extra RAM |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | reference |
| handwritten_runtime | - | - | - | - | 18 (+2) | 0/0 | 1160 (+20) | 532 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_alt1_bool | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | - | - | - | - | 18 (+0) | 0/0 | 1160 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt4_filter | - | - | - | - | 23 (+7) | 0/0 | 1164 (+24) | 520 (+4) | 0/0 | - | FAIL: publish path, no extra RAM |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 88 `collapse_publish`
- 40 `collapse_setup`
- 12 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::gate`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_alt3_static (bytes)</summary>

- 96 `collapse_publish`
- 1 `sub0x::detail::g_canceled`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_alt3_tls (bytes)</summary>

- 256 `tlsBlock`
- 112 `collapse_publish`
- 4 `__aeabi_read_tp`
- 1 `sub0x::detail::g_canceled`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_alt4_filter (bytes)</summary>

- 108 `collapse_publish`
- 32 `collapse_setup`
- 1 `(anonymous namespace)::stopped`

</details>

## Case: cancellation_filtered

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 26.8 (+0.0) | 20 (+0) | 16 (+0) | 31 (+0) | 0/0 | 2447 (+0) | 624 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | 27.2 (+0.3) | 20 (+0) | 16 (+0) | 34 (+3) | 0/0 | 2447 (+0) | 624 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_alt2_token | ok | 27.2 (+0.3) | 20 (+0) | 16 (+0) | 34 (+3) | 0/0 | 2447 (+0) | 624 (+0) | 0/0 | - | FAIL: publish path |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 12.3 (+0.0) | 20 (+0) | 16 (+0) | 10 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | 12.3 (+0.0) | 20 (+0) | 16 (+0) | 10 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | 12.3 (+0.0) | 20 (+0) | 16 (+0) | 10 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | PASS |

<details><summary>gcc-O2: largest symbols added by sub0x_alt1_bool (bytes)</summary>

- 121 `collapse_publish`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_alt2_token (bytes)</summary>

- 123 `collapse_publish`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 28.8 (+0.0) | 18 (+0) | 14 (+0) | 37 (+0) | 0/0 | 2202 (+0) | 664 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | 28.0 (-0.8) | 18 (+0) | 14 (+0) | 34 (-3) | 0/0 | 2186 (-16) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | 28.0 (-0.8) | 18 (+0) | 14 (+0) | 34 (-3) | 0/0 | 2186 (-16) | 664 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.3 (+0.0) | 17 (+0) | 14 (+0) | 15 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | 14.3 (+0.0) | 17 (+0) | 14 (+0) | 15 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | 14.3 (+0.0) | 17 (+0) | 14 (+0) | 15 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | PASS |

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 36 (+0) | 0/0 | 1192 (+0) | 516 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | - | - | - | - | 40 (+4) | 0/0 | 1204 (+12) | 516 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_alt2_token | - | - | - | - | 40 (+4) | 0/0 | 1204 (+12) | 516 (+0) | 0/0 | - | FAIL: publish path |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 18 (+0) | 0/0 | 1144 (+0) | 516 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | - | - | - | - | 18 (+0) | 0/0 | 1144 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | - | - | - | - | 18 (+0) | 0/0 | 1144 (+0) | 516 (+0) | 0/0 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_alt1_bool (bytes)</summary>

- 104 `collapse_publish`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_alt2_token (bytes)</summary>

- 104 `collapse_publish`

</details>

## Case: cross_file

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 61.0 (+0.0) | 21 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2617 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 74.0 (+13.0) | 31 (+10) | 16 (+0) | 22 (-28) | 2/1 | 2798 (+181) | 672 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 61.0 (+0.0) | 27 (+6) | 16 (+0) | 50 (+0) | 4/0 | 2649 (+32) | 656 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 130.0 (+69.0) | 56 (+35) | 128 (+112) | 73 (+23) | 2/2 | 7235 (+4618) | 1104 (+472) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 114.0 (+53.0) | 56 (+35) | 128 (+112) | 62 (+12) | 1/2 | 7055 (+4438) | 1096 (+464) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 61.0 (+0.0) | 27 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2649 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 61.0 (+0.0) | 21 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2617 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 74.0 (+0.0) | 31 (+0) | 16 (+0) | 22 (+0) | 2/1 | 2798 (+0) | 672 (+0) | 0/55 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 36.0 (+0.0) | 21 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2574 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 49.0 (+13.0) | 31 (+10) | 16 (+0) | 22 (-12) | 2/1 | 2750 (+176) | 672 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 36.0 (+0.0) | 27 (+6) | 16 (+0) | 34 (+0) | 4/0 | 2606 (+32) | 656 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 105.0 (+69.0) | 56 (+35) | 128 (+112) | 73 (+39) | 2/2 | 7187 (+4613) | 1104 (+472) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 89.0 (+53.0) | 56 (+35) | 128 (+112) | 62 (+28) | 1/2 | 7007 (+4433) | 1096 (+464) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 36.0 (+0.0) | 27 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2606 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 36.0 (+0.0) | 21 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2574 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 49.0 (+0.0) | 31 (+0) | 16 (+0) | 22 (+0) | 2/1 | 2750 (+0) | 672 (+0) | 0/55 | - | PASS |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 105 `collapse_setup`
- 68 `app::Sensor::send(unsigned int)`
- 54 `(anonymous namespace)::deliverNode(void const*, app::Sample const&)`
- 24 `(anonymous namespace)::node`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 77 `collapse_setup`
- 24 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `app::Logger::~Logger()`
- 862 `app::Controller::~Controller()`
- 359 `collapse_setup`
- 317 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 48 `vtable for sub0::Subscribe<app::Sample>`
- 48 `vtable for app::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `app::Logger::~Logger()`
- 862 `app::Controller::~Controller()`
- 359 `collapse_setup`
- 251 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 48 `vtable for sub0::Subscribe<app::Sample>`
- 48 `vtable for app::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 55 `sub0x::Sink<app::Sample>::Sink<sub0x::Wiring<app::Controller, app::Controller, app::Logger>, 0>(sub0x::Wiring<app::Controller, app::Controller, app::Logger>&)::{lambda(void const*, app::Sample const&)#1}::_FUN(void const*, app::Sample const&)`
- 24 `(anonymous namespace)::bus`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 55.0 (+0.0) | 19 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2298 (+0) | 672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 67.0 (+12.0) | 29 (+10) | 14 (+0) | 15 (-27) | 1/1 | 2493 (+195) | 712 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 55.0 (+0.0) | 25 (+6) | 14 (+0) | 42 (+0) | 3/0 | 2346 (+48) | 696 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 150.0 (+95.0) | 57 (+38) | 129 (+115) | 66 (+24) | 1/2 | 5022 (+2724) | 1137 (+465) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 133.0 (+78.0) | 57 (+38) | 129 (+115) | 55 (+13) | 0/2 | 4866 (+2568) | 1129 (+457) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 55.0 (+0.0) | 25 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2346 (+0) | 696 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 55.0 (+0.0) | 19 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2298 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 66.0 (-1.0) | 29 (+0) | 14 (+0) | 14 (-1) | 1/1 | 2490 (-3) | 712 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 19 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2256 (+0) | 672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 39.0 (+12.0) | 29 (+10) | 14 (+0) | 15 (-9) | 1/1 | 2461 (+205) | 712 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 27.0 (+0.0) | 25 (+6) | 14 (+0) | 24 (+0) | 3/0 | 2304 (+48) | 696 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 122.0 (+95.0) | 57 (+38) | 129 (+115) | 66 (+42) | 1/2 | 4990 (+2734) | 1137 (+465) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 105.0 (+78.0) | 57 (+38) | 129 (+115) | 55 (+31) | 0/2 | 4834 (+2578) | 1129 (+457) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 27.0 (+0.0) | 25 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2304 (+0) | 696 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 27.0 (+0.0) | 19 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2256 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 38.0 (-1.0) | 29 (+0) | 14 (+0) | 14 (-1) | 1/1 | 2458 (-3) | 712 (+0) | 0/0 | - | PASS |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 101 `collapse_setup`
- 49 `(anonymous namespace)::deliverNode(void const*, app::Sample const&)`
- 24 `app::Sensor::send(unsigned int)`
- 24 `(anonymous namespace)::node`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>clang-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 73 `collapse_setup`
- 8 `_ZN12_GLOBAL__N_16sensorE.2`
- 8 `_ZN12_GLOBAL__N_16sensorE.1`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 324 `sub0::Subscribe<app::Sample>::~Subscribe()`
- 317 `collapse_setup`
- 265 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<app::Sample>`
- 48 `vtable for app::Logger`
- 48 `vtable for app::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 324 `sub0::Subscribe<app::Sample>::~Subscribe()`
- 317 `collapse_setup`
- 219 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<app::Sample>`
- 48 `vtable for app::Logger`
- 48 `vtable for app::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 49 `_ZZN5sub0x4SinkIN3app6SampleEEC1INS_6WiringIJNS1_10ControllerES6_NS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 24 `(anonymous namespace)::bus`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 43 (+0) | 3/0 | 1220 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 17 (-26) | 1/1 | 1264 (+44) | 540 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 42 (-1) | 3/0 | 1224 (+4) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 178 (+135) | 5/2 | 3024 (+1804) | 948 (+428) | 641/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 58 (+15) | 4/2 | 2704 (+1484) | 948 (+428) | 601/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 42 (+0) | 3/0 | 1224 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 43 (+0) | 3/0 | 1220 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 17 (+0) | 1/1 | 1264 (+0) | 540 (+0) | 0/32 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 27 (+0) | 3/0 | 1176 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 17 (-10) | 1/1 | 1220 (+44) | 540 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 26 (-1) | 3/0 | 1180 (+4) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 178 (+151) | 5/2 | 2984 (+1808) | 948 (+428) | 641/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 58 (+31) | 4/2 | 2664 (+1488) | 948 (+428) | 601/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 26 (+0) | 3/0 | 1180 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 27 (+0) | 3/0 | 1176 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 17 (+0) | 1/1 | 1220 (+0) | 540 (+0) | 0/32 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 60 `collapse_setup`
- 32 `(anonymous namespace)::deliverNode(void const*, app::Sample const&)`
- 18 `app::Sensor::send(unsigned long)`
- 12 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 44 `collapse_setup`
- 12 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 340 `sub0::Subscribe<app::Sample>::~Subscribe()`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 152 `sub0::detail::Broker<app::Sample>::publish(app::Sample const&) const`
- 96 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 340 `sub0::Subscribe<app::Sample>::~Subscribe()`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 112 `sub0::detail::Broker<app::Sample>::publish(app::Sample const&) const`
- 96 `collapse_setup`
- 80 `sub0::Subscribe<app::Sample>::Subscribe()`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 32 `sub0x::Sink<app::Sample>::Sink<sub0x::Wiring<app::Controller, app::Controller, app::Logger>, 0>(sub0x::Wiring<app::Controller, app::Controller, app::Logger>&)::{lambda(void const*, app::Sample const&)#1}::_FUN(void const*, app::Sample const&)`
- 12 `(anonymous namespace)::bus`

</details>

### gcc-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 11 (+0) | 25 (+0) | 0/0 | 2388 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 48.0 (+18.0) | 31 (+10) | 11 (+0) | 16 (-9) | 1/1 | 2561 (+173) | 680 (+48) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 33.0 (+3.0) | 27 (+6) | 11 (+0) | 28 (+3) | 0/0 | 2425 (+37) | 664 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 130.0 (+100.0) | 56 (+35) | 128 (+117) | 73 (+48) | 2/2 | 7228 (+4840) | 1128 (+496) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 114.0 (+84.0) | 56 (+35) | 128 (+117) | 62 (+37) | 1/2 | 7048 (+4660) | 1120 (+488) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 33.0 (+0.0) | 27 (+0) | 11 (+0) | 28 (+0) | 0/0 | 2425 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 30.0 (+0.0) | 21 (+0) | 11 (+0) | 25 (+0) | 0/0 | 2388 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 48.0 (+0.0) | 31 (+0) | 11 (+0) | 16 (+0) | 1/1 | 2561 (+0) | 680 (+0) | 0/76 | - | PASS |

### gcc-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 11 (+0) | 4 (+0) | 0/0 | 2321 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 24.0 (+15.0) | 31 (+10) | 11 (+0) | 16 (+12) | 1/1 | 2497 (+176) | 680 (+48) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 27 (+6) | 11 (+0) | 5 (+1) | 0/0 | 2356 (+35) | 664 (+32) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 105.0 (+96.0) | 56 (+35) | 128 (+117) | 73 (+69) | 2/2 | 7180 (+4859) | 1128 (+496) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 89.0 (+80.0) | 56 (+35) | 128 (+117) | 62 (+58) | 1/2 | 7000 (+4679) | 1120 (+488) | 235/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 11 (+0) | 5 (+0) | 0/0 | 2356 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 21 (+0) | 11 (+0) | 4 (+0) | 0/0 | 2321 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 24.0 (+0.0) | 31 (+0) | 11 (+0) | 16 (+0) | 1/1 | 2497 (+0) | 680 (+0) | 0/11 | - | PASS |

<details><summary>gcc-O2-lto: largest symbols added by handwritten_erased (bytes)</summary>

- 105 `collapse_setup`
- 76 `(anonymous namespace)::deliverNode(void const*, app::Sample const&)`
- 24 `(anonymous namespace)::node`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2-lto: largest symbols added by handwritten_runtime (bytes)</summary>

- 91 `collapse_publish`
- 77 `collapse_setup`
- 24 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2-lto: largest symbols added by sub0pub_virtual (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `app::Logger::~Logger()`
- 862 `app::Controller::~Controller()`
- 620 `main`
- 359 `collapse_setup`
- 317 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 48 `vtable for sub0::Subscribe<app::Sample>`

</details>

<details><summary>gcc-O2-lto: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `app::Logger::~Logger()`
- 862 `app::Controller::~Controller()`
- 620 `main`
- 359 `collapse_setup`
- 251 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 48 `vtable for sub0::Subscribe<app::Sample>`

</details>

<details><summary>gcc-O2-lto: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 76 `sub0x::Sink<app::Sample>::Sink<sub0x::Wiring<app::Controller, app::Controller, app::Logger>, 0>(sub0x::Wiring<app::Controller, app::Controller, app::Logger>&)::{lambda(void const*, app::Sample const&)#1}::_FUN(void const*, app::Sample const&)`
- 24 `(anonymous namespace)::bus`

</details>

### clang-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 41.0 (+10.0) | 26 (+8) | 10 (+0) | 11 (-16) | 0/1 | 2246 (+132) | 696 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 146.0 (+115.0) | 54 (+36) | 129 (+119) | 65 (+38) | 1/2 | 4982 (+2868) | 1137 (+473) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 129.0 (+98.0) | 54 (+36) | 129 (+119) | 54 (+27) | 0/2 | 4826 (+2712) | 1129 (+465) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 40.0 (-1.0) | 26 (+0) | 10 (+0) | 36 (+25) | 1/0 | 2224 (-22) | 696 (+0) | 0/0 | - | FAIL: publish path |

### clang-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2004 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 11.0 (+3.0) | 25 (+10) | 10 (+0) | 7 (+3) | 1/0 | 2136 (+132) | 696 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 17 (+2) | 10 (+0) | 4 (+0) | 0/0 | 2024 (+20) | 664 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 121.0 (+113.0) | 53 (+38) | 129 (+119) | 65 (+61) | 1/2 | 4950 (+2946) | 1137 (+481) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 104.0 (+96.0) | 53 (+38) | 129 (+119) | 54 (+50) | 0/2 | 4794 (+2790) | 1129 (+473) | 600/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 17 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2024 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 15 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2004 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 11.0 (+0.0) | 25 (+0) | 10 (+0) | 8 (+1) | 1/0 | 2136 (+0) | 696 (+0) | 0/0 | - | PASS |

<details><summary>clang-O2-lto: largest symbols added by handwritten_erased (bytes)</summary>

- 87 `collapse_setup`
- 77 `(anonymous namespace)::deliverNode(void const*, app::Sample const&)`
- 24 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::logger`
- 4 `(anonymous namespace)::controllerB`
- 4 `(anonymous namespace)::controllerA`

</details>

<details><summary>clang-O2-lto: largest symbols added by sub0pub_virtual (bytes)</summary>

- 554 `main`
- 324 `sub0::Subscribe<app::Sample>::~Subscribe()`
- 302 `collapse_setup`
- 265 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<app::Sample>`
- 48 `vtable for app::Logger`

</details>

<details><summary>clang-O2-lto: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 554 `main`
- 324 `sub0::Subscribe<app::Sample>::~Subscribe()`
- 302 `collapse_setup`
- 219 `collapse_publish`
- 72 `sub0::detail::Broker<app::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<app::Sample>`
- 48 `vtable for app::Logger`

</details>

<details><summary>clang-O2-lto: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 71 `_ZZN5sub0x4SinkIN3app6SampleEEC1INS_6WiringIJNS1_10ControllerES6_NS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 24 `(anonymous namespace)::bus`

</details>

### cm33-gcc-Os-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1172 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-15) | 0/1 | 1216 (+44) | 540 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 28 (-2) | 0/0 | 1176 (+4) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+138) | 4/2 | 2960 (+1788) | 952 (+432) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+20) | 3/2 | 2644 (+1472) | 952 (+432) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 28 (+0) | 0/0 | 1176 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1172 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1216 (+0) | 540 (+0) | 0/56 | - | PASS |

### cm33-gcc-Os-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+3) | 0/1 | 1172 (+56) | 540 (+20) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1128 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+156) | 4/2 | 2916 (+1800) | 952 (+432) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+38) | 3/2 | 2600 (+1484) | 952 (+432) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1128 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1172 (+0) | 540 (+0) | 0/10 | - | PASS |

<details><summary>cm33-gcc-Os-lto: largest symbols added by handwritten_erased (bytes)</summary>

- 76 `main`
- 60 `collapse_setup`
- 56 `(anonymous namespace)::deliverNode(void const*, app::Sample const&)`
- 12 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os-lto: largest symbols added by handwritten_runtime (bytes)</summary>

- 44 `collapse_setup`
- 12 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os-lto: largest symbols added by sub0pub_virtual (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 164 `collapse_publish`
- 156 `sub0::detail::Broker<app::Sample>::unsubscribe(sub0::Subscribe<app::Sample>*)`
- 84 `main`

</details>

<details><summary>cm33-gcc-Os-lto: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 156 `sub0::detail::Broker<app::Sample>::unsubscribe(sub0::Subscribe<app::Sample>*)`
- 128 `collapse_publish`
- 84 `main`
- 84 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os-lto: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 56 `sub0x::Sink<app::Sample>::Sink<sub0x::Wiring<app::Controller, app::Controller, app::Logger>, 0>(sub0x::Wiring<app::Controller, app::Controller, app::Logger>&)::{lambda(void const*, app::Sample const&)#1}::_FUN(void const*, app::Sample const&)`
- 12 `(anonymous namespace)::bus`

</details>

## Case: dynamic_subscriptions

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 60.8 (+0.0) | 30 (+0) | 32 (+0) | 87 (+0) | 2/2 | 3676 (+0) | 840 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | 93.3 (+32.5) | 33 (+3) | 49 (+17) | 212 (+125) | 4/2 | 6872 (+3196) | 1080 (+240) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 80.0 (+19.3) | 33 (+3) | 49 (+17) | 169 (+82) | 2/2 | 6500 (+2824) | 1072 (+232) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 107.3 (+46.5) | 37 (+7) | 66 (+34) | 255 (+168) | 5/2 | 5478 (+1802) | 1008 (+168) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 73.8 (+13.0) | 37 (+7) | 53 (+21) | 165 (+78) | 3/2 | 4714 (+1038) | 968 (+128) | 0/545 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 51.0 (+0.0) | 30 (+0) | 32 (+0) | 87 (+0) | 2/2 | 3600 (+0) | 840 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | 83.5 (+32.5) | 33 (+3) | 49 (+17) | 212 (+125) | 4/2 | 6796 (+3196) | 1080 (+240) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 70.3 (+19.3) | 33 (+3) | 49 (+17) | 169 (+82) | 2/2 | 6424 (+2824) | 1072 (+232) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 97.5 (+46.5) | 37 (+7) | 66 (+34) | 255 (+168) | 5/2 | 5402 (+1802) | 1008 (+168) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 64.0 (+13.0) | 37 (+7) | 53 (+21) | 165 (+78) | 3/2 | 4638 (+1038) | 968 (+128) | 0/545 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 993 `collapse_publish`
- 862 `(anonymous namespace)::Probe::~Probe()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 304 `collapse_teardown`
- 93 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Probe`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 862 `(anonymous namespace)::Probe::~Probe()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 745 `collapse_publish`
- 304 `collapse_teardown`
- 93 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Probe`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 675 `collapse_publish`
- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 103 `collapse_setup`
- 75 `(anonymous namespace)::Probe::~Probe()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 103 `collapse_setup`
- 75 `(anonymous namespace)::Probe::~Probe()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 45 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 57.0 (+0.0) | 26 (+0) | 30 (+0) | 142 (+0) | 0/2 | 3671 (+0) | 848 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | 103.3 (+46.3) | 31 (+5) | 54 (+24) | 215 (+73) | 4/4 | 5113 (+1442) | 1105 (+257) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 93.8 (+36.8) | 31 (+5) | 54 (+24) | 195 (+53) | 2/4 | 4913 (+1242) | 1097 (+249) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 115.0 (+58.0) | 35 (+9) | 58 (+28) | 288 (+146) | 4/4 | 5027 (+1356) | 1008 (+160) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 63.3 (+6.2) | 35 (+9) | 50 (+20) | 160 (+18) | 2/2 | 4223 (+552) | 968 (+120) | 0/571 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 45.8 (+0.0) | 26 (+0) | 30 (+0) | 142 (+0) | 0/2 | 3630 (+0) | 848 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | 92.0 (+46.3) | 31 (+5) | 54 (+24) | 215 (+73) | 4/4 | 5072 (+1442) | 1105 (+257) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 82.5 (+36.8) | 31 (+5) | 54 (+24) | 195 (+53) | 2/4 | 4872 (+1242) | 1097 (+249) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 103.8 (+58.0) | 35 (+9) | 58 (+28) | 288 (+146) | 4/4 | 4995 (+1365) | 1008 (+160) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 52.0 (+6.2) | 35 (+9) | 50 (+20) | 160 (+18) | 2/2 | 4191 (+561) | 968 (+120) | 0/571 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 587 `collapse_publish`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 98 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Probe`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 98 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Probe`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 645 `collapse_publish`
- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 103 `collapse_setup`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Probe`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 103 `collapse_setup`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for (anonymous namespace)::Probe`
- 40 `vtable for (anonymous namespace)::Controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 162 (+0) | 4/1 | 1636 (+0) | 548 (+0) | 0/0 | - | reference |
| sub0pub_virtual | - | - | - | - | 203 (+41) | 7/2 | 2940 (+1304) | 924 (+376) | 357/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 83 (-79) | 6/2 | 2628 (+992) | 924 (+376) | 321/0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | - | - | - | - | 245 (+83) | 5/2 | 2968 (+1332) | 912 (+364) | 0/476 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 237 (+75) | 6/1 | 2516 (+880) | 648 (+100) | 0/316 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 138 (+0) | 2/0 | 1520 (+0) | 548 (+0) | 0/0 | - | reference |
| sub0pub_virtual | - | - | - | - | 203 (+65) | 7/2 | 2896 (+1376) | 924 (+376) | 357/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 83 (-55) | 6/2 | 2580 (+1060) | 924 (+376) | 321/0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | - | - | - | - | 245 (+107) | 5/2 | 2924 (+1404) | 912 (+364) | 0/476 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 237 (+99) | 6/1 | 2472 (+952) | 648 (+100) | 0/316 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 236 `memcpy`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 132 `sub0::detail::Broker<(anonymous namespace)::Sample>::publish((anonymous namespace)::Sample const&) const`
- 100 `collapse_publish`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 100 `collapse_publish`
- 96 `sub0::detail::Broker<(anonymous namespace)::Sample>::publish((anonymous namespace)::Sample const&) const`
- 76 `_impure_data`
- 72 `sbrk_aligned`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 132 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::publish((anonymous namespace)::Sample const&, void const*, sub0x::PublishReport*) const [clone .constprop.0]`
- 76 `_impure_data`
- 72 `sbrk_aligned`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 256 `_malloc_r`
- 172 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 60 `(anonymous namespace)::Probe::~Probe()`
- 60 `(anonymous namespace)::Controller::~Controller()`
- 48 `sub0x::Subscribe<(anonymous namespace)::Sample>::trySubscribe() [clone .isra.0]`

</details>

## Case: filters

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 35.5 (+16.5) | 26 (+8) | 16 (+0) | 17 (-1) | 1/1 | 2531 (+164) | 656 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 19.0 (+0.0) | 22 (+4) | 16 (+0) | 18 (+0) | 0/0 | 2399 (+32) | 640 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 122.5 (+103.5) | 44 (+26) | 89 (+73) | 73 (+55) | 2/2 | 6744 (+4377) | 1096 (+480) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 94.5 (+75.5) | 44 (+26) | 89 (+73) | 54 (+36) | 1/2 | 6532 (+4165) | 1088 (+472) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 19.0 (+0.0) | 22 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2399 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 35.5 (+0.0) | 26 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2531 (+0) | 656 (+0) | 0/55 | - | PASS |
| sub0x_dynamic | ok | 131.5 (+112.5) | 55 (+37) | 113 (+97) | 79 (+61) | 2/2 | 5298 (+2931) | 1024 (+408) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 86.5 (+67.5) | 55 (+37) | 86 (+70) | 49 (+31) | 1/2 | 4842 (+2475) | 1008 (+392) | 0/563 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 26 (+8) | 16 (+0) | 17 (+13) | 1/1 | 2483 (+164) | 656 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 22 (+4) | 16 (+0) | 4 (+0) | 0/0 | 2351 (+32) | 640 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 111.0 (+103.0) | 44 (+26) | 89 (+73) | 73 (+69) | 2/2 | 6668 (+4349) | 1096 (+480) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 83.0 (+75.0) | 44 (+26) | 89 (+73) | 54 (+50) | 1/2 | 6456 (+4137) | 1088 (+472) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 22 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2351 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 26 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2483 (+0) | 656 (+0) | 0/5 | - | PASS |
| sub0x_dynamic | ok | 120.0 (+112.0) | 55 (+37) | 113 (+97) | 79 (+75) | 2/2 | 5222 (+2903) | 1024 (+408) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 75.0 (+67.0) | 55 (+37) | 86 (+70) | 49 (+45) | 1/2 | 4766 (+2447) | 1008 (+392) | 0/563 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 79 `collapse_publish`
- 61 `collapse_setup`
- 55 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::sensor`
- 16 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 33 `collapse_setup`
- 16 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 862 `(anonymous namespace)::EvenMonitor::~EvenMonitor()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 716 `collapse_teardown`
- 312 `collapse_publish`
- 188 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::EvenMonitor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 862 `(anonymous namespace)::EvenMonitor::~EvenMonitor()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 716 `collapse_teardown`
- 222 `collapse_publish`
- 188 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::EvenMonitor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 55 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 351 `collapse_publish`
- 194 `collapse_setup`
- 75 `(anonymous namespace)::EvenMonitor::~EvenMonitor()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 52 `collapse_teardown`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 194 `collapse_setup`
- 162 `collapse_publish`
- 75 `(anonymous namespace)::EvenMonitor::~EvenMonitor()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2> > >::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 52 `collapse_teardown`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 31.0 (+9.0) | 22 (+6) | 14 (+0) | 12 (-7) | 0/1 | 2218 (+96) | 688 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 113.0 (+91.0) | 42 (+26) | 92 (+78) | 66 (+47) | 1/2 | 4937 (+2815) | 1121 (+465) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 94.5 (+72.5) | 42 (+26) | 92 (+78) | 55 (+36) | 0/2 | 4781 (+2659) | 1113 (+457) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 30.0 (-1.0) | 22 (+0) | 14 (+0) | 30 (+18) | 1/0 | 2233 (+15) | 688 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic | ok | 121.0 (+99.0) | 53 (+37) | 110 (+96) | 70 (+51) | 1/2 | 4826 (+2704) | 1024 (+368) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 77.5 (+55.5) | 53 (+37) | 94 (+80) | 41 (+22) | 0/2 | 4366 (+2244) | 1008 (+352) | 0/582 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 22 (+6) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+32) | 688 (+32) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 99.5 (+91.5) | 42 (+26) | 92 (+78) | 66 (+62) | 1/2 | 4905 (+2831) | 1121 (+465) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 81.0 (+73.0) | 42 (+26) | 92 (+78) | 55 (+51) | 0/2 | 4749 (+2675) | 1113 (+457) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 22 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+0) | 688 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 107.5 (+99.5) | 53 (+37) | 110 (+96) | 70 (+66) | 1/2 | 4794 (+2720) | 1024 (+368) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 64.0 (+56.0) | 53 (+37) | 94 (+80) | 41 (+37) | 0/2 | 4334 (+2260) | 1008 (+352) | 0/582 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 48 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 43 `collapse_setup`
- 16 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 265 `collapse_publish`
- 186 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::EvenMonitor`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 219 `collapse_publish`
- 186 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::EvenMonitor`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 48 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerENS1_11EvenMonitorEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 16 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 282 `collapse_publish`
- 199 `collapse_setup`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::EvenMonitor`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 199 `collapse_setup`
- 115 `collapse_publish`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2> > >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::EvenMonitor`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-4) | 0/1 | 1192 (+64) | 528 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 19 (+0) | 0/0 | 1148 (+20) | 520 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+149) | 4/2 | 2932 (+1804) | 932 (+424) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+31) | 3/2 | 2616 (+1488) | 932 (+424) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 19 (+0) | 0/0 | 1148 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1192 (+0) | 528 (+0) | 0/40 | - | PASS |
| sub0x_dynamic | - | - | - | - | 171 (+152) | 3/2 | 2960 (+1832) | 920 (+412) | 0/344 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 34 (+15) | 0/2 | 2528 (+1400) | 656 (+148) | 0/284 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1156 (+60) | 528 (+20) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 7 (+0) | 0/0 | 1116 (+20) | 520 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+161) | 4/2 | 2884 (+1788) | 932 (+424) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+43) | 3/2 | 2568 (+1472) | 932 (+424) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 7 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1156 (+0) | 528 (+0) | 0/2 | - | PASS |
| sub0x_dynamic | - | - | - | - | 171 (+164) | 3/2 | 2912 (+1816) | 920 (+412) | 0/344 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 34 (+27) | 0/2 | 2480 (+1384) | 656 (+148) | 0/284 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 40 `collapse_setup`
- 40 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 24 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 164 `collapse_publish`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 104 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 128 `collapse_publish`
- 104 `collapse_setup`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 40 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 160 `collapse_publish`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 256 `_malloc_r`
- 254 `memmove`
- 172 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 72 `collapse_publish`
- 60 `(anonymous namespace)::EvenMonitor::~EvenMonitor()`

</details>

## Case: large_payload

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 67.0 (+0.0) | 19 (+0) | 16 (+0) | 43 (+0) | 1/0 | 2529 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 77.0 (+10.0) | 27 (+8) | 16 (+0) | 31 (-12) | 1/1 | 2669 (+140) | 656 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 70.0 (+3.0) | 23 (+4) | 16 (+0) | 46 (+3) | 1/0 | 2577 (+48) | 640 (+16) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 141.0 (+74.0) | 45 (+26) | 89 (+73) | 74 (+31) | 2/1 | 6787 (+4258) | 1096 (+472) | 255/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 117.0 (+50.0) | 45 (+26) | 89 (+73) | 59 (+16) | 1/1 | 6591 (+4062) | 1088 (+464) | 255/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 70.0 (+0.0) | 23 (+0) | 16 (+0) | 46 (+0) | 1/0 | 2577 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 67.0 (+0.0) | 19 (+0) | 16 (+0) | 43 (+0) | 1/0 | 2529 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 76.0 (-1.0) | 27 (+0) | 16 (+0) | 30 (-1) | 1/1 | 2669 (+0) | 656 (+0) | 0/70 | - | PASS |
| sub0x_dynamic | ok | 147.0 (+80.0) | 57 (+38) | 113 (+97) | 78 (+35) | 2/1 | 5343 (+2814) | 1024 (+400) | 0/711 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 103.0 (+36.0) | 57 (+38) | 86 (+70) | 49 (+6) | 1/1 | 4771 (+2242) | 984 (+360) | 0/543 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 19 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2335 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 59.0 (+50.0) | 27 (+8) | 16 (+0) | 31 (+25) | 1/1 | 2605 (+270) | 656 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 23 (+4) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+32) | 640 (+16) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 123.0 (+114.0) | 45 (+26) | 89 (+73) | 74 (+68) | 2/1 | 6723 (+4388) | 1096 (+472) | 255/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 99.0 (+90.0) | 45 (+26) | 89 (+73) | 59 (+53) | 1/1 | 6527 (+4192) | 1088 (+464) | 255/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 10.0 (+0.0) | 23 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 19 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2335 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 57.0 (-2.0) | 27 (+0) | 16 (+0) | 30 (-1) | 1/1 | 2605 (+0) | 656 (+0) | 0/11 | - | PASS |
| sub0x_dynamic | ok | 129.0 (+120.0) | 57 (+38) | 113 (+97) | 78 (+72) | 2/1 | 5279 (+2944) | 1024 (+400) | 0/711 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 85.0 (+76.0) | 57 (+38) | 86 (+70) | 49 (+43) | 1/1 | 4707 (+2372) | 984 (+360) | 0/543 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 71 `collapse_setup`
- 69 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Frame const&)`
- 16 `(anonymous namespace)::sensor`
- 16 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 185 `collapse_publish`
- 43 `collapse_setup`
- 16 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 716 `collapse_teardown`
- 327 `collapse_publish`
- 183 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Frame>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Frame>`
- 48 `vtable for (anonymous namespace)::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 716 `collapse_teardown`
- 256 `collapse_publish`
- 183 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Frame>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Frame>`
- 48 `vtable for (anonymous namespace)::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 175 `collapse_publish`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 70 `sub0x::Sink<(anonymous namespace)::Frame>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Frame const&)#1}::_FUN(void const*, (anonymous namespace)::Frame const&)`
- 16 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 423 `sub0x::Subscribe<(anonymous namespace)::Frame>::disconnect()`
- 360 `collapse_publish`
- 214 `collapse_setup`
- 75 `(anonymous namespace)::Logger::~Logger()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Frame, sub0x::Builtin>::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Frame, true>`
- 52 `collapse_teardown`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 281 `sub0x::Subscribe<(anonymous namespace)::Frame>::disconnect()`
- 214 `collapse_setup`
- 183 `collapse_publish`
- 75 `(anonymous namespace)::Logger::~Logger()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Frame, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Frame, false>`
- 52 `collapse_teardown`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 26.0 (+0.0) | 17 (+0) | 14 (+0) | 23 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 53.0 (+27.0) | 23 (+6) | 14 (+0) | 27 (+4) | 0/1 | 2389 (+251) | 688 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 26.0 (+0.0) | 17 (+0) | 14 (+0) | 23 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 138.0 (+112.0) | 43 (+26) | 92 (+78) | 82 (+59) | 1/2 | 5049 (+2911) | 1121 (+457) | 594/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 120.0 (+94.0) | 43 (+26) | 92 (+78) | 71 (+48) | 0/2 | 4897 (+2759) | 1113 (+449) | 594/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 26.0 (+0.0) | 17 (+0) | 14 (+0) | 23 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 26.0 (+0.0) | 17 (+0) | 14 (+0) | 23 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 52.0 (-1.0) | 23 (+0) | 14 (+0) | 48 (+21) | 1/0 | 2399 (+10) | 688 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic | ok | 148.0 (+122.0) | 54 (+37) | 110 (+96) | 87 (+64) | 1/2 | 4929 (+2791) | 1024 (+360) | 0/761 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 78.0 (+52.0) | 54 (+37) | 94 (+80) | 44 (+21) | 0/1 | 4313 (+2175) | 984 (+320) | 0/569 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 34.0 (+25.0) | 23 (+6) | 14 (+0) | 27 (+23) | 0/1 | 2341 (+267) | 688 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 118.0 (+109.0) | 43 (+26) | 92 (+78) | 82 (+78) | 1/2 | 5005 (+2931) | 1121 (+457) | 594/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 100.0 (+91.0) | 43 (+26) | 92 (+78) | 71 (+67) | 0/2 | 4853 (+2779) | 1113 (+449) | 594/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 33.0 (-1.0) | 23 (+0) | 14 (+0) | 29 (+2) | 1/0 | 2346 (+5) | 688 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 128.0 (+119.0) | 54 (+37) | 110 (+96) | 87 (+83) | 1/2 | 4897 (+2823) | 1024 (+360) | 0/761 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 58.0 (+49.0) | 54 (+37) | 94 (+80) | 44 (+40) | 0/1 | 4281 (+2207) | 984 (+320) | 0/569 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 122 `collapse_publish`
- 60 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Frame const&)`
- 53 `collapse_setup`
- 16 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::logger`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 347 `collapse_publish`
- 300 `sub0::Subscribe<(anonymous namespace)::Frame>::~Subscribe()`
- 196 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Frame>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Frame>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`
- 42 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Frame>`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Frame>::~Subscribe()`
- 298 `collapse_publish`
- 196 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Frame>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Frame>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`
- 42 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Frame>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 59 `_ZZN5sub0x4SinkIN12_GLOBAL__N_15FrameEEC1INS_6WiringIJNS1_10ControllerENS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 16 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 482 `sub0x::Subscribe<(anonymous namespace)::Frame>::~Subscribe()`
- 362 `collapse_publish`
- 209 `collapse_setup`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Frame, sub0x::Builtin>::global_`
- 65 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Frame, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Frame>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 309 `sub0x::Subscribe<(anonymous namespace)::Frame>::~Subscribe()`
- 209 `collapse_setup`
- 165 `collapse_publish`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Frame, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 65 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Frame, false>`
- 43 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Frame>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Frame>`
- 40 `vtable for (anonymous namespace)::Logger`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 35 (+0) | 0/0 | 1172 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 22 (-13) | 0/1 | 1224 (+52) | 532 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 37 (+2) | 0/0 | 1192 (+20) | 524 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 169 (+134) | 4/1 | 2936 (+1764) | 932 (+420) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 51 (+16) | 3/1 | 2620 (+1448) | 932 (+420) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 37 (+0) | 0/0 | 1192 (+0) | 524 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 35 (+0) | 0/0 | 1172 (+0) | 512 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 22 (+0) | 0/1 | 1224 (+0) | 532 (+0) | 0/52 | - | PASS |
| sub0x_dynamic | - | - | - | - | 169 (+134) | 3/1 | 2960 (+1788) | 920 (+408) | 0/344 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 32 (-3) | 0/1 | 2512 (+1340) | 660 (+148) | 0/276 | operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 22 (+10) | 0/1 | 1184 (+68) | 532 (+20) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1132 (+16) | 524 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 169 (+157) | 4/1 | 2892 (+1776) | 932 (+420) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 51 (+39) | 3/1 | 2576 (+1460) | 932 (+420) | 225/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1132 (+0) | 524 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 512 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 22 (+0) | 0/1 | 1184 (+0) | 532 (+0) | 0/10 | - | PASS |
| sub0x_dynamic | - | - | - | - | 169 (+157) | 3/1 | 2916 (+1800) | 920 (+408) | 0/344 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 32 (+20) | 0/1 | 2468 (+1352) | 660 (+148) | 0/276 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 52 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Frame const&)`
- 44 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 88 `collapse_publish`
- 28 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `collapse_publish`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Frame>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Frame>*) [clone .constprop.0]`
- 108 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Frame>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Frame>*) [clone .constprop.0]`
- 132 `collapse_publish`
- 108 `collapse_setup`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 52 `sub0x::Sink<(anonymous namespace)::Frame>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Frame const&)#1}::_FUN(void const*, (anonymous namespace)::Frame const&)`
- 8 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Frame>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 160 `collapse_publish`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 256 `_malloc_r`
- 254 `memmove`
- 172 `sub0x::Subscribe<(anonymous namespace)::Frame>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 60 `collapse_setup`
- 60 `(anonymous namespace)::Logger::~Logger()`

</details>

## Case: many_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 202.0 (+0.0) | 50 (+0) | 16 (+0) | 198 (+0) | 0/0 | 3231 (+0) | 744 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 282.0 (+80.0) | 118 (+68) | 16 (+0) | 17 (-181) | 1/1 | 4067 (+836) | 1016 (+272) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_loop | ok | 332.0 (+130.0) | 80 (+30) | 67 (+51) | 18 (-180) | 0/0 | 2518 (-713) | 760 (+16) | 0/0 | - | reference; FAIL: publish instr, setup instr, teardown instr, no extra RAM |
| handwritten_runtime | ok | 296.0 (+94.0) | 114 (+64) | 16 (+0) | 291 (+93) | 0/0 | 4015 (+784) | 1016 (+272) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 434.0 (+232.0) | 344 (+294) | 1733 (+1717) | 65 (-133) | 4/0 | 20353 (+17122) | 1680 (+936) | 625/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 336.0 (+134.0) | 344 (+294) | 1733 (+1717) | 23 (-175) | 0/0 | 20001 (+16770) | 1672 (+928) | 440/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 296.0 (+0.0) | 114 (+0) | 16 (+0) | 291 (+0) | 0/0 | 4015 (+0) | 1016 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 202.0 (+0.0) | 50 (+0) | 16 (+0) | 198 (+0) | 0/0 | 3231 (+0) | 744 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 282.0 (+0.0) | 118 (+0) | 16 (+0) | 17 (+0) | 1/1 | 4067 (+0) | 1016 (+0) | 0/834 | - | PASS |
| sub0x_dynamic | ok | 528.0 (+326.0) | 591 (+541) | 2200 (+2184) | 16 (-182) | 1/1 | 8175 (+4944) | 1624 (+880) | 0/1103 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 336.0 (+134.0) | 591 (+541) | 1963 (+1947) | 23 (-175) | 0/0 | 7551 (+4320) | 1592 (+848) | 0/737 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 50 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2639 (+0) | 744 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 118 (+68) | 16 (+0) | 17 (+13) | 1/1 | 3235 (+596) | 1016 (+272) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_loop | ok | 59.0 (+51.0) | 80 (+30) | 67 (+51) | 10 (+6) | 0/0 | 2502 (-137) | 760 (+16) | 0/0 | - | reference; FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 114 (+64) | 16 (+0) | 4 (+0) | 0/0 | 3087 (+448) | 1016 (+272) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 8.0 (+0.0) | 344 (+294) | 1733 (+1717) | 4 (+0) | 0/0 | 19905 (+17266) | 1672 (+928) | 440/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 344 (+294) | 1733 (+1717) | 4 (+0) | 0/0 | 19905 (+17266) | 1672 (+928) | 440/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 114 (+0) | 16 (+0) | 4 (+0) | 0/0 | 3087 (+0) | 1016 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 50 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2639 (+0) | 744 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 118 (+0) | 16 (+0) | 17 (+0) | 1/1 | 3235 (+0) | 1016 (+0) | 0/5 | - | PASS |
| sub0x_dynamic | ok | 8.0 (+0.0) | 591 (+541) | 1963 (+1947) | 4 (+0) | 0/0 | 7547 (+4908) | 1608 (+864) | 0/755 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 8.0 (+0.0) | 591 (+541) | 1963 (+1947) | 4 (+0) | 0/0 | 7455 (+4816) | 1592 (+848) | 0/737 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 834 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 801 `collapse_setup`
- 256 `(anonymous namespace)::node`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_loop (bytes)</summary>

- 128 `(anonymous namespace)::controllers`
- 34 `collapse_teardown`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 944 `collapse_publish`
- 773 `collapse_setup`
- 256 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 12769 `collapse_teardown`
- 2789 `collapse_setup`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 264 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 185 `sub0::detail::Broker<(anonymous namespace)::Sample>::publish((anonymous namespace)::Sample const&) const [clone .isra.0]`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 12769 `collapse_teardown`
- 2789 `collapse_setup`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 264 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 42 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 834 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 256 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 3032 `collapse_setup`
- 622 `collapse_teardown`
- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 264 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::Capacity<32u> > >::global_`
- 198 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::Capacity<32u> > >::publish((anonymous namespace)::Sample const&, void const*, sub0x::PublishReport*) const [clone .constprop.0]`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 3032 `collapse_setup`
- 622 `collapse_teardown`
- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 264 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter, sub0x::Capacity<32u> > >::global_`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 45 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 256.0 (+0.0) | 48 (+0) | 14 (+0) | 251 (+0) | 0/0 | 3210 (+0) | 784 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 274.0 (+18.0) | 114 (+66) | 14 (+0) | 12 (-239) | 0/1 | 3722 (+512) | 1048 (+264) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_loop | ok | 230.0 (-26.0) | 32 (-16) | 14 (+0) | 38 (-213) | 0/0 | 2453 (-757) | 784 (+0) | 0/0 | - | reference; PASS |
| handwritten_runtime | ok | 265.0 (+9.0) | 112 (+64) | 14 (+0) | 262 (+11) | 0/0 | 3594 (+384) | 1040 (+256) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 1007.0 (+751.0) | 353 (+305) | 1880 (+1866) | 65 (-186) | 1/2 | 7518 (+4308) | 1721 (+937) | 788/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 1001.0 (+745.0) | 353 (+305) | 1880 (+1866) | 55 (-196) | 0/2 | 7362 (+4152) | 1713 (+929) | 788/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 265.0 (+0.0) | 112 (+0) | 14 (+0) | 262 (+0) | 0/0 | 3594 (+0) | 1040 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 256.0 (+0.0) | 48 (+0) | 14 (+0) | 251 (+0) | 0/0 | 3210 (+0) | 784 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 273.0 (-1.0) | 114 (+0) | 14 (+0) | 270 (+258) | 1/0 | 3730 (+8) | 1048 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic | ok | 1137.0 (+881.0) | 594 (+546) | 2348 (+2334) | 70 (-181) | 1/2 | 7652 (+4442) | 1624 (+840) | 0/955 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 599.0 (+343.0) | 594 (+546) | 2032 (+2018) | 28 (-223) | 0/1 | 7060 (+3850) | 1592 (+808) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 114 (+98) | 14 (+0) | 4 (+0) | 0/0 | 2842 (+768) | 1048 (+392) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_loop | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| handwritten_runtime | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 719.0 (+711.0) | 353 (+337) | 1880 (+1866) | 65 (+61) | 1/2 | 7502 (+5428) | 1721 (+1065) | 788/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 713.0 (+705.0) | 353 (+337) | 1880 (+1866) | 55 (+51) | 0/2 | 7346 (+5272) | 1713 (+1057) | 788/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 114 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2842 (+0) | 1048 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 849.0 (+841.0) | 594 (+578) | 2348 (+2334) | 70 (+66) | 1/2 | 7636 (+5562) | 1624 (+968) | 0/955 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 311.0 (+303.0) | 594 (+578) | 2032 (+2018) | 28 (+24) | 0/1 | 7044 (+4970) | 1592 (+936) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 809 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 783 `collapse_setup`
- 256 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::c9`
- 4 `(anonymous namespace)::c8`
- 4 `(anonymous namespace)::c7`
- 4 `(anonymous namespace)::c6`

</details>

<details><summary>clang-O2: largest symbols added by handwritten_loop (bytes)</summary>

- 128 `(anonymous namespace)::controllers`

</details>

<details><summary>clang-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 769 `collapse_setup`
- 256 `(anonymous namespace)::sensor`
- 4 `(anonymous namespace)::c9`
- 4 `(anonymous namespace)::c8`
- 4 `(anonymous namespace)::c7`
- 4 `(anonymous namespace)::c6`
- 4 `(anonymous namespace)::c5`
- 4 `(anonymous namespace)::c4`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 2785 `collapse_setup`
- 402 `collapse_teardown`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 264 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 2785 `collapse_setup`
- 402 `collapse_teardown`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 264 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 809 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerES6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_S6_EEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSA_ENUlPKvRKS2_E_8__invokeESH_SJ_`
- 256 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 3054 `collapse_setup`
- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 386 `collapse_teardown`
- 264 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::Capacity<32u> > >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 3054 `collapse_setup`
- 386 `collapse_teardown`
- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 264 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter, sub0x::Capacity<32u> > >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for (anonymous namespace)::Controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 143 (+0) | 32/0 | 1824 (+0) | 636 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-128) | 0/1 | 1940 (+116) | 772 (+136) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_loop | - | - | - | - | 21 (-122) | 0/0 | 1156 (-668) | 636 (+0) | 0/0 | - | reference; PASS |
| handwritten_runtime | - | - | - | - | 114 (-29) | 32/0 | 1840 (+16) | 764 (+128) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 146 (+3) | 2/0 | 3488 (+1664) | 1128 (+492) | 200/0 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 31 (-112) | 1/0 | 3184 (+1360) | 1128 (+492) | 200/0 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 114 (+0) | 32/0 | 1840 (+0) | 764 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 143 (+0) | 32/0 | 1824 (+0) | 636 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1940 (+0) | 772 (+0) | 0/330 | - | PASS |
| sub0x_dynamic | - | - | - | - | 148 (+5) | 2/0 | 3552 (+1728) | 1384 (+748) | 0/224 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 31 (-112) | 1/0 | 3168 (+1344) | 1120 (+484) | 0/212 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1416 (+0) | 636 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1592 (+176) | 772 (+136) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_loop | - | - | - | - | 11 (+4) | 0/0 | 1128 (-288) | 636 (+0) | 0/0 | - | reference; FAIL: publish path |
| handwritten_runtime | - | - | - | - | 7 (+0) | 0/0 | 1552 (+136) | 764 (+128) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 7 (+0) | 0/0 | 3128 (+1712) | 1128 (+492) | 200/0 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 3128 (+1712) | 1128 (+492) | 200/0 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 7 (+0) | 0/0 | 1552 (+0) | 764 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1416 (+0) | 636 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1592 (+0) | 772 (+0) | 0/2 | - | PASS |
| sub0x_dynamic | - | - | - | - | 7 (+0) | 0/0 | 3188 (+1772) | 1384 (+748) | 0/224 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 7 (+0) | 0/0 | 3112 (+1696) | 1120 (+484) | 0/212 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 476 `collapse_setup`
- 330 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 128 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_loop (bytes)</summary>

- 128 `(anonymous namespace)::controllers`
- 10 `collapse_teardown`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 460 `collapse_setup`
- 128 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 540 `collapse_setup`
- 340 `(anonymous namespace)::Controller::~Controller()`
- 324 `collapse_teardown`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 132 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 540 `collapse_setup`
- 340 `(anonymous namespace)::Controller::~Controller()`
- 324 `collapse_teardown`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 132 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 330 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 128 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 524 `collapse_setup`
- 500 `(anonymous namespace)::Controller::~Controller()`
- 324 `collapse_teardown`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 524 `collapse_setup`
- 388 `(anonymous namespace)::Controller::~Controller()`
- 324 `collapse_teardown`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 132 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter, sub0x::Capacity<32ul> > >::global_`
- 76 `_impure_data`

</details>

## Case: multi_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 52.0 (+22.0) | 31 (+10) | 16 (+0) | 17 (-9) | 1/1 | 2627 (+196) | 672 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 38.0 (+8.0) | 27 (+6) | 16 (+0) | 35 (+9) | 0/0 | 2495 (+64) | 656 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_spike | ok | 129.0 (+99.0) | 56 (+35) | 128 (+112) | 65 (+39) | 2/1 | 7228 (+4797) | 1112 (+480) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 129.0 (+99.0) | 56 (+35) | 128 (+112) | 65 (+39) | 2/1 | 7228 (+4797) | 1112 (+480) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 99.0 (+69.0) | 56 (+35) | 128 (+112) | 45 (+19) | 1/1 | 7024 (+4593) | 1104 (+472) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 52.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2627 (+0) | 672 (+0) | 0/99 | - | PASS |
| sub0x_dynamic | ok | 137.0 (+107.0) | 76 (+55) | 165 (+149) | 68 (+42) | 2/1 | 5355 (+2924) | 1040 (+408) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 86.0 (+56.0) | 76 (+55) | 124 (+108) | 37 (+11) | 1/1 | 4767 (+2336) | 1000 (+368) | 0/545 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 24.0 (+15.0) | 31 (+10) | 16 (+0) | 17 (+11) | 1/1 | 2531 (+164) | 672 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 27 (+6) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+32) | 656 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 104.0 (+95.0) | 56 (+35) | 128 (+112) | 65 (+59) | 2/1 | 7180 (+4813) | 1112 (+480) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 104.0 (+95.0) | 56 (+35) | 128 (+112) | 65 (+59) | 2/1 | 7180 (+4813) | 1112 (+480) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 74.0 (+65.0) | 56 (+35) | 128 (+112) | 45 (+39) | 1/1 | 6976 (+4609) | 1104 (+472) | 257/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 24.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2531 (+0) | 672 (+0) | 0/11 | - | PASS |
| sub0x_dynamic | ok | 112.0 (+103.0) | 76 (+55) | 165 (+149) | 68 (+62) | 2/1 | 5307 (+2940) | 1040 (+408) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 61.0 (+52.0) | 76 (+55) | 124 (+108) | 37 (+31) | 1/1 | 4719 (+2352) | 1000 (+368) | 0/545 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 105 `collapse_setup`
- 99 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::node`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 114 `collapse_publish`
- 77 `collapse_setup`
- 24 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 330 `collapse_setup`
- 276 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 330 `collapse_setup`
- 276 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 1116 `collapse_teardown`
- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Controller::~Controller()`
- 330 `collapse_setup`
- 199 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 99 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 319 `collapse_setup`
- 308 `collapse_publish`
- 75 `(anonymous namespace)::Logger::~Logger()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 71 `collapse_teardown`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 319 `collapse_setup`
- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 126 `collapse_publish`
- 75 `(anonymous namespace)::Logger::~Logger()`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 71 `collapse_teardown`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 45.0 (+12.0) | 27 (+8) | 14 (+0) | 12 (-17) | 0/1 | 2314 (+144) | 704 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | reference; PASS |
| sub0pub_spike | ok | 150.0 (+117.0) | 55 (+36) | 135 (+121) | 66 (+37) | 1/2 | 5023 (+2853) | 1137 (+465) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 150.0 (+117.0) | 55 (+36) | 135 (+121) | 66 (+37) | 1/2 | 5023 (+2853) | 1137 (+465) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 133.0 (+100.0) | 55 (+36) | 135 (+121) | 55 (+26) | 0/2 | 4867 (+2697) | 1129 (+457) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 44.0 (-1.0) | 27 (+0) | 14 (+0) | 41 (+29) | 1/0 | 2314 (+0) | 704 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic | ok | 163.0 (+130.0) | 73 (+54) | 165 (+151) | 70 (+41) | 1/2 | 4904 (+2734) | 1040 (+368) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 79.0 (+46.0) | 73 (+54) | 141 (+127) | 28 (-1) | 0/1 | 4288 (+2118) | 1000 (+328) | 0/571 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 18.0 (+9.0) | 27 (+10) | 14 (+0) | 12 (+8) | 0/1 | 2234 (+160) | 704 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 19 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+16) | 672 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 122.0 (+113.0) | 55 (+38) | 135 (+121) | 66 (+62) | 1/2 | 4980 (+2906) | 1137 (+473) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 122.0 (+113.0) | 55 (+38) | 135 (+121) | 66 (+62) | 1/2 | 4980 (+2906) | 1137 (+473) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 105.0 (+96.0) | 55 (+38) | 135 (+121) | 55 (+51) | 0/2 | 4824 (+2750) | 1129 (+465) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 17.0 (-1.0) | 27 (+0) | 14 (+0) | 14 (+2) | 1/0 | 2239 (+5) | 704 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 135.0 (+126.0) | 73 (+56) | 165 (+151) | 70 (+66) | 1/2 | 4872 (+2798) | 1040 (+376) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 51.0 (+42.0) | 73 (+56) | 141 (+127) | 28 (+24) | 0/1 | 4256 (+2182) | 1000 (+336) | 0/571 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 87 `collapse_setup`
- 81 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::logger`
- 4 `(anonymous namespace)::controllerB`
- 4 `(anonymous namespace)::controllerA`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 301 `collapse_setup`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 265 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 301 `collapse_setup`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 265 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 301 `collapse_setup`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 219 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 54 `collapse_teardown`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 81 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerES6_NS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 310 `collapse_setup`
- 282 `collapse_publish`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Logger`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 310 `collapse_setup`
- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for (anonymous namespace)::Logger`
- 40 `vtable for (anonymous namespace)::Controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-15) | 0/1 | 1240 (+52) | 540 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 32 (+2) | 0/0 | 1200 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 162 (+132) | 4/1 | 2940 (+1752) | 948 (+428) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 162 (+132) | 4/1 | 2940 (+1752) | 948 (+428) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 44 (+14) | 3/1 | 2624 (+1436) | 948 (+428) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1240 (+0) | 540 (+0) | 0/68 | - | PASS |
| sub0x_dynamic | - | - | - | - | 162 (+132) | 3/1 | 2964 (+1776) | 936 (+416) | 0/344 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 23 (-7) | 0/1 | 2524 (+1336) | 676 (+156) | 0/276 | operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+3) | 0/1 | 1184 (+48) | 540 (+20) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1148 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 162 (+150) | 4/1 | 2900 (+1764) | 948 (+428) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 162 (+150) | 4/1 | 2900 (+1764) | 948 (+428) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 44 (+32) | 3/1 | 2584 (+1448) | 948 (+428) | 265/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1184 (+0) | 540 (+0) | 0/10 | - | PASS |
| sub0x_dynamic | - | - | - | - | 162 (+150) | 3/1 | 2924 (+1788) | 936 (+416) | 0/344 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 23 (+11) | 0/1 | 2484 (+1348) | 676 (+156) | 0/276 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 68 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 60 `collapse_setup`
- 12 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 44 `collapse_setup`
- 12 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_spike (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 148 `collapse_publish`
- 84 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 148 `collapse_publish`
- 84 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 112 `collapse_publish`
- 84 `collapse_setup`
- 76 `_impure_data`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 68 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 12 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 132 `collapse_publish`
- 84 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 256 `_malloc_r`
- 254 `memmove`
- 172 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 84 `collapse_setup`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 60 `(anonymous namespace)::Logger::~Logger()`

</details>

## Case: multi_types

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 32.0 (+0.0) | 19 (+0) | 16 (+0) | 28 (+0) | 0/0 | 2399 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 67.0 (+35.0) | 32 (+13) | 16 (+0) | 28 (+0) | 1/2 | 2719 (+320) | 704 (+80) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 36.0 (+4.0) | 25 (+6) | 16 (+0) | 33 (+5) | 0/0 | 2463 (+64) | 656 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 187.0 (+155.0) | 71 (+52) | 171 (+155) | 105 (+77) | 3/2 | 12050 (+9651) | 1576 (+952) | 516/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 134.0 (+102.0) | 71 (+52) | 171 (+155) | 71 (+43) | 1/2 | 11770 (+9371) | 1568 (+944) | 516/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 36.0 (+0.0) | 25 (+0) | 16 (+0) | 33 (+0) | 0/0 | 2463 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 32.0 (+0.0) | 19 (+0) | 16 (+0) | 28 (+0) | 0/0 | 2399 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 67.0 (+0.0) | 32 (+0) | 16 (+0) | 28 (+0) | 1/2 | 2719 (+0) | 704 (+0) | 0/113 | - | PASS |
| sub0x_dynamic | ok | 204.0 (+172.0) | 96 (+77) | 214 (+198) | 120 (+92) | 3/2 | 7759 (+5360) | 1424 (+800) | 0/1428 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 114.0 (+82.0) | 96 (+77) | 160 (+144) | 57 (+29) | 1/2 | 6759 (+4360) | 1352 (+728) | 0/1092 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 19 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2335 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 37.0 (+28.0) | 32 (+13) | 16 (+0) | 28 (+22) | 1/2 | 2623 (+288) | 704 (+80) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 25 (+6) | 16 (+0) | 6 (+0) | 0/0 | 2383 (+48) | 656 (+32) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 156.0 (+147.0) | 71 (+52) | 171 (+155) | 105 (+99) | 3/2 | 11866 (+9531) | 1576 (+952) | 516/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 103.0 (+94.0) | 71 (+52) | 171 (+155) | 71 (+65) | 1/2 | 11586 (+9251) | 1568 (+944) | 516/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 10.0 (+0.0) | 25 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2383 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 19 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2335 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 37.0 (+0.0) | 32 (+0) | 16 (+0) | 28 (+0) | 1/2 | 2623 (+0) | 704 (+0) | 0/17 | - | PASS |
| sub0x_dynamic | ok | 173.0 (+164.0) | 96 (+77) | 214 (+198) | 120 (+114) | 3/2 | 7575 (+5240) | 1424 (+800) | 0/1428 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 83.0 (+74.0) | 96 (+77) | 160 (+144) | 57 (+51) | 1/2 | 6575 (+4240) | 1352 (+728) | 0/1092 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 111 `collapse_publish`
- 106 `collapse_setup`
- 65 `(anonymous namespace)::deliverSample(void const*, (anonymous namespace)::Sample const&)`
- 48 `(anonymous namespace)::deliverCommand(void const*, (anonymous namespace)::Command const&)`
- 32 `(anonymous namespace)::sensor`
- 24 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::controller`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 99 `collapse_publish`
- 57 `collapse_setup`
- 24 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::controller`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 1975 `(anonymous namespace)::Controller::~Controller()`
- 1478 `collapse_teardown`
- 1348 `non-virtual thunk to (anonymous namespace)::Controller::~Controller()`
- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Actuator::~Actuator()`
- 491 `collapse_publish`
- 487 `collapse_setup`
- 104 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 1975 `(anonymous namespace)::Controller::~Controller()`
- 1478 `collapse_teardown`
- 1348 `non-virtual thunk to (anonymous namespace)::Controller::~Controller()`
- 862 `(anonymous namespace)::Logger::~Logger()`
- 862 `(anonymous namespace)::Actuator::~Actuator()`
- 487 `collapse_setup`
- 329 `collapse_publish`
- 104 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 65 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 48 `sub0x::Sink<(anonymous namespace)::Command>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>&)::{lambda(void const*, (anonymous namespace)::Command const&)#1}::_FUN(void const*, (anonymous namespace)::Command const&)`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 563 `collapse_publish`
- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 423 `sub0x::Subscribe<(anonymous namespace)::Command>::disconnect()`
- 419 `collapse_setup`
- 184 `(anonymous namespace)::Controller::~Controller()`
- 149 `non-virtual thunk to (anonymous namespace)::Controller::~Controller()`
- 121 `collapse_teardown`
- 104 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 419 `collapse_setup`
- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 281 `sub0x::Subscribe<(anonymous namespace)::Command>::disconnect()`
- 201 `collapse_publish`
- 184 `(anonymous namespace)::Controller::~Controller()`
- 149 `non-virtual thunk to (anonymous namespace)::Controller::~Controller()`
- 121 `collapse_teardown`
- 88 `vtable for (anonymous namespace)::Controller`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 34.0 (+0.0) | 17 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2154 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 59.0 (+25.0) | 26 (+9) | 14 (+0) | 19 (-12) | 0/2 | 2382 (+228) | 712 (+48) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 34.0 (+0.0) | 17 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2154 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 221.0 (+187.0) | 76 (+59) | 165 (+151) | 119 (+88) | 2/4 | 7400 (+5246) | 1593 (+929) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 185.0 (+151.0) | 76 (+59) | 165 (+151) | 96 (+65) | 0/4 | 7184 (+5030) | 1585 (+921) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 34.0 (+0.0) | 17 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2154 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 34.0 (+0.0) | 17 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2154 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 59.0 (+0.0) | 26 (+0) | 14 (+0) | 57 (+38) | 2/0 | 2388 (+6) | 712 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic | ok | 243.0 (+209.0) | 93 (+76) | 205 (+191) | 130 (+99) | 2/4 | 7166 (+5012) | 1384 (+720) | 0/1528 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 107.0 (+73.0) | 93 (+76) | 173 (+159) | 46 (+15) | 0/2 | 6058 (+3904) | 1312 (+648) | 0/1144 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 18.0 (+9.0) | 26 (+9) | 14 (+0) | 12 (+8) | 0/1 | 2218 (+144) | 712 (+48) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0pub_virtual | ok | 185.0 (+176.0) | 76 (+59) | 165 (+151) | 119 (+115) | 2/4 | 7313 (+5239) | 1593 (+929) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 149.0 (+140.0) | 76 (+59) | 165 (+151) | 96 (+92) | 0/4 | 7097 (+5023) | 1585 (+921) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 17.0 (-1.0) | 26 (+0) | 14 (+0) | 14 (+2) | 1/0 | 2224 (+6) | 712 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 207.0 (+198.0) | 93 (+76) | 205 (+191) | 130 (+126) | 2/4 | 7086 (+5012) | 1384 (+720) | 0/1528 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 71.0 (+62.0) | 93 (+76) | 173 (+159) | 46 (+42) | 0/2 | 5978 (+3904) | 1312 (+648) | 0/1144 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 74 `collapse_setup`
- 56 `(anonymous namespace)::deliverSample(void const*, (anonymous namespace)::Sample const&)`
- 39 `(anonymous namespace)::deliverCommand(void const*, (anonymous namespace)::Command const&)`
- 24 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.2`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::logger`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 531 `collapse_publish`
- 349 `collapse_setup`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 300 `sub0::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 104 `vtable for (anonymous namespace)::Controller`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 72 `sub0::detail::Broker<(anonymous namespace)::Command>::state_`
- 66 `collapse_teardown`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 426 `collapse_publish`
- 349 `collapse_setup`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 300 `sub0::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 104 `vtable for (anonymous namespace)::Controller`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 72 `sub0::detail::Broker<(anonymous namespace)::Command>::state_`
- 66 `collapse_teardown`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 56 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerENS1_6LoggerENS1_8ActuatorEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSC_ENUlPKvRKS2_E_8__invokeESJ_SL_`
- 39 `_ZZN5sub0x4SinkIN12_GLOBAL__N_17CommandEEC1INS_6WiringIJNS1_10ControllerENS1_6LoggerENS1_8ActuatorEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSC_ENUlPKvRKS2_E_8__invokeESJ_SL_`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 540 `collapse_publish`
- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 482 `sub0x::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 413 `collapse_setup`
- 104 `vtable for (anonymous namespace)::Controller`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Command, sub0x::Builtin>::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Command, true>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 413 `collapse_setup`
- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 309 `sub0x::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 151 `collapse_publish`
- 88 `vtable for (anonymous namespace)::Controller`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Command, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Command, false>`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 4/0 | 1168 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 23 (-7) | 0/2 | 1268 (+100) | 548 (+36) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 32 (+2) | 4/0 | 1196 (+28) | 532 (+20) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 209 (+179) | 7/2 | 3512 (+2344) | 1004 (+492) | 450/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 76 (+46) | 5/2 | 3160 (+1992) | 1004 (+492) | 450/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 4/0 | 1196 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 4/0 | 1168 (+0) | 512 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 23 (+0) | 0/2 | 1268 (+0) | 548 (+0) | 0/56 | - | PASS |
| sub0x_dynamic | - | - | - | - | 207 (+177) | 6/2 | 3560 (+2392) | 980 (+468) | 0/688 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 40 (+10) | 0/2 | 2972 (+1804) | 712 (+200) | 0/552 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 23 (+11) | 0/2 | 1204 (+88) | 548 (+36) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1140 (+24) | 532 (+20) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 209 (+197) | 7/2 | 3440 (+2324) | 1004 (+492) | 450/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 76 (+64) | 5/2 | 3092 (+1976) | 1004 (+492) | 450/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1140 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 512 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 23 (+0) | 0/2 | 1204 (+0) | 548 (+0) | 0/12 | - | PASS |
| sub0x_dynamic | - | - | - | - | 207 (+195) | 6/2 | 3492 (+2376) | 980 (+468) | 0/688 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 40 (+28) | 0/2 | 2904 (+1788) | 712 (+200) | 0/552 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 64 `collapse_setup`
- 34 `(anonymous namespace)::deliverSample(void const*, (anonymous namespace)::Sample const&)`
- 22 `(anonymous namespace)::deliverCommand(void const*, (anonymous namespace)::Command const&)`
- 16 `(anonymous namespace)::sensor`
- 12 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::controller`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 64 `collapse_publish`
- 36 `collapse_setup`
- 12 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::controller`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 276 `collapse_publish`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 208 `collapse_setup`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 208 `collapse_setup`
- 204 `collapse_publish`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`
- 156 `sub0::detail::Broker<(anonymous namespace)::Command>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Command>*) [clone .constprop.0]`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 34 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 22 `sub0x::Sink<(anonymous namespace)::Command>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger, (anonymous namespace)::Actuator>&)::{lambda(void const*, (anonymous namespace)::Command const&)#1}::_FUN(void const*, (anonymous namespace)::Command const&)`
- 12 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 256 `tlsBlock`
- 256 `collapse_publish`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 228 `sub0x::Subscribe<(anonymous namespace)::Command>::disconnect() [clone .constprop.0]`
- 168 `_free_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 256 `_malloc_r`
- 254 `memmove`
- 172 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 172 `sub0x::Subscribe<(anonymous namespace)::Command>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 116 `collapse_setup`
- 108 `(anonymous namespace)::Controller::~Controller()`
- 92 `collapse_publish`

</details>

## Case: nested_publish

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 56.0 (+0.0) | 18 (+0) | 16 (+0) | 85 (+0) | 4/0 | 2683 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 56.0 (+0.0) | 26 (+8) | 16 (+0) | 85 (+0) | 4/0 | 2731 (+48) | 656 (+40) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 209.0 (+153.0) | 62 (+44) | 132 (+116) | 65 (-20) | 2/1 | 10496 (+7813) | 1520 (+904) | 507/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 209.0 (+153.0) | 62 (+44) | 132 (+116) | 65 (-20) | 2/1 | 10496 (+7813) | 1520 (+904) | 507/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 54.5 (-1.5) | 26 (+0) | 16 (+0) | 246 (+161) | 4/0 | 3267 (+536) | 656 (+0) | 0/269 | - | FAIL: publish path, no Sub0Pub retained |
| sub0x_b2_static | ok | 54.5 (-1.5) | 18 (+0) | 16 (+0) | 246 (+161) | 4/0 | 3219 (+536) | 616 (+0) | 0/269 | - | FAIL: publish path, no Sub0Pub retained |
| sub0x_dynamic | ok | 233.0 (+177.0) | 74 (+56) | 161 (+145) | 68 (-17) | 2/1 | 7484 (+4801) | 1384 (+768) | 0/1547 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 137.5 (+81.5) | 74 (+56) | 121 (+105) | 37 (-48) | 1/1 | 6364 (+3681) | 1320 (+704) | 0/1211 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 8.0 (+0.0) | 26 (+8) | 16 (+0) | 4 (+0) | 0/0 | 2367 (+48) | 656 (+40) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 167.0 (+159.0) | 62 (+44) | 132 (+116) | 65 (+61) | 2/1 | 10340 (+8021) | 1520 (+904) | 507/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 167.0 (+159.0) | 62 (+44) | 132 (+116) | 65 (+61) | 2/1 | 10340 (+8021) | 1520 (+904) | 507/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 26 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2367 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 170.0 (+162.0) | 74 (+56) | 148 (+132) | 68 (+64) | 2/1 | 7084 (+4765) | 1376 (+760) | 0/1397 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 92.5 (+84.5) | 74 (+56) | 121 (+105) | 37 (+33) | 1/1 | 6208 (+3889) | 1320 (+704) | 0/1211 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 147 `(anonymous namespace)::Node::publish((anonymous namespace)::Sample const&) const [clone .isra.0]`
- 61 `collapse_setup`
- 24 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::relay`
- 1 `(anonymous namespace)::tail`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 1252 `non-virtual thunk to (anonymous namespace)::Relay::~Relay()`
- 1040 `collapse_teardown`
- 862 `(anonymous namespace)::Tail::~Tail()`
- 862 `(anonymous namespace)::Relay::~Relay()`
- 862 `(anonymous namespace)::Actuator::~Actuator()`
- 417 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 276 `collapse_publish`
- 252 `collapse_setup`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 1252 `non-virtual thunk to (anonymous namespace)::Relay::~Relay()`
- 1040 `collapse_teardown`
- 862 `(anonymous namespace)::Tail::~Tail()`
- 862 `(anonymous namespace)::Relay::~Relay()`
- 862 `(anonymous namespace)::Actuator::~Actuator()`
- 417 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 276 `collapse_publish`
- 252 `collapse_setup`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 516 `collapse_publish`
- 269 `void sub0x::Wiring<(anonymous namespace)::Relay, (anonymous namespace)::Actuator, (anonymous namespace)::Tail>::publish<(anonymous namespace)::Sample>((anonymous namespace)::Sample const&) const [clone .isra.0]`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 516 `collapse_publish`
- 269 `void sub0x::StaticWiring<&(anonymous namespace)::relay, &(anonymous namespace)::actuator, &(anonymous namespace)::tail>::publish<(anonymous namespace)::Sample>((anonymous namespace)::Sample const&)`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 524 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 423 `sub0x::Subscribe<(anonymous namespace)::Command>::disconnect()`
- 308 `collapse_publish`
- 292 `collapse_setup`
- 78 `collapse_teardown`
- 75 `(anonymous namespace)::Tail::~Tail()`
- 75 `(anonymous namespace)::Relay::~Relay()`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 292 `collapse_setup`
- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 281 `sub0x::Subscribe<(anonymous namespace)::Command>::disconnect()`
- 213 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 78 `collapse_teardown`
- 75 `(anonymous namespace)::Tail::~Tail()`
- 75 `(anonymous namespace)::Relay::~Relay()`
- 75 `(anonymous namespace)::Actuator::~Actuator()`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 61.5 (+0.0) | 16 (+0) | 14 (+0) | 48 (+0) | 2/0 | 2254 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 65.5 (+4.0) | 24 (+8) | 14 (+0) | 51 (+3) | 2/0 | 2318 (+64) | 696 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 283.5 (+222.0) | 65 (+49) | 129 (+115) | 66 (+18) | 1/2 | 7530 (+5276) | 1553 (+897) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 283.5 (+222.0) | 65 (+49) | 129 (+115) | 66 (+18) | 1/2 | 7530 (+5276) | 1553 (+897) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 65.5 (+0.0) | 24 (+0) | 14 (+0) | 51 (+0) | 2/0 | 2318 (+0) | 696 (+0) | 0/108 | - | PASS |
| sub0x_b2_static | ok | 65.5 (+4.0) | 16 (+0) | 14 (+0) | 55 (+7) | 2/0 | 2292 (+38) | 656 (+0) | 0/0 | - | FAIL: publish instr, publish path |
| sub0x_dynamic | ok | 303.5 (+242.0) | 72 (+56) | 155 (+141) | 70 (+22) | 1/2 | 7107 (+4853) | 1360 (+704) | 0/1645 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 136.5 (+75.0) | 72 (+56) | 131 (+117) | 28 (-20) | 0/1 | 5791 (+3537) | 1296 (+640) | 0/1261 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 8.0 (+0.0) | 24 (+8) | 14 (+0) | 4 (+0) | 0/0 | 2122 (+48) | 696 (+40) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 247.5 (+239.5) | 65 (+49) | 129 (+115) | 66 (+62) | 1/2 | 7473 (+5399) | 1553 (+897) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 247.5 (+239.5) | 65 (+49) | 129 (+115) | 66 (+62) | 1/2 | 7473 (+5399) | 1553 (+897) | 1194/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 24 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2122 (+0) | 696 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 267.5 (+259.5) | 72 (+56) | 155 (+141) | 70 (+66) | 1/2 | 7043 (+4969) | 1360 (+704) | 0/1645 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 102.0 (+94.0) | 72 (+56) | 131 (+117) | 28 (+24) | 0/1 | 5743 (+3669) | 1296 (+640) | 0/1261 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 107 `(anonymous namespace)::Node::publish((anonymous namespace)::Sample const&) const`
- 57 `collapse_setup`
- 33 `collapse_publish`
- 24 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::relay`
- 1 `(anonymous namespace)::tail`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 562 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 300 `sub0::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 298 `collapse_setup`
- 265 `collapse_publish`
- 152 `non-virtual thunk to (anonymous namespace)::Relay::~Relay()`
- 112 `vtable for (anonymous namespace)::Relay`
- 82 `collapse_teardown`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 562 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 300 `sub0::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 298 `collapse_setup`
- 265 `collapse_publish`
- 152 `non-virtual thunk to (anonymous namespace)::Relay::~Relay()`
- 112 `vtable for (anonymous namespace)::Relay`
- 82 `collapse_teardown`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 108 `void sub0x::Wiring<(anonymous namespace)::Relay, (anonymous namespace)::Actuator, (anonymous namespace)::Tail>::publish<(anonymous namespace)::Sample>((anonymous namespace)::Sample const&) const`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 95 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 59 `collapse_publish`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 572 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 482 `sub0x::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 302 `collapse_setup`
- 282 `collapse_publish`
- 72 `typeinfo for (anonymous namespace)::Relay`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Command, sub0x::Builtin>::global_`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 309 `sub0x::Subscribe<(anonymous namespace)::Command>::~Subscribe()`
- 302 `collapse_setup`
- 167 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 85 `collapse_publish`
- 72 `typeinfo for (anonymous namespace)::Relay`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Command, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 40 (+0) | 2/0 | 1180 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_runtime | - | - | - | - | 40 (+0) | 2/0 | 1212 (+32) | 528 (+20) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 170 (+130) | 5/1 | 3388 (+2208) | 988 (+480) | 421/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 170 (+130) | 5/1 | 3388 (+2208) | 988 (+480) | 421/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 41 (+1) | 6/0 | 1216 (+4) | 528 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 40 (+0) | 2/0 | 1180 (+0) | 508 (+0) | 0/72 | - | PASS |
| sub0x_dynamic | - | - | - | - | 11 (-29) | 0/1 | 3420 (+2240) | 964 (+456) | 0/544 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 23 (-17) | 0/1 | 2888 (+1708) | 700 (+192) | 0/332 | operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 14 (+0) | 2/0 | 1112 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_runtime | - | - | - | - | 7 (-7) | 0/0 | 1128 (+16) | 528 (+20) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 170 (+156) | 5/1 | 3284 (+2172) | 988 (+480) | 421/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 170 (+156) | 5/1 | 3284 (+2172) | 988 (+480) | 421/0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 7 (+0) | 0/0 | 1128 (+0) | 528 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 14 (+0) | 2/0 | 1112 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | - | - | - | - | 11 (-3) | 0/1 | 3316 (+2204) | 964 (+456) | 0/544 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 23 (+9) | 0/1 | 2788 (+1676) | 700 (+192) | 0/332 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 72 `(anonymous namespace)::Node::publish((anonymous namespace)::Sample const&) const [clone .isra.0]`
- 36 `collapse_setup`
- 12 `(anonymous namespace)::node`
- 4 `(anonymous namespace)::relay`
- 1 `(anonymous namespace)::tail`
- 1 `(anonymous namespace)::actuator`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 340 `(anonymous namespace)::Actuator::~Actuator()`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 172 `collapse_setup`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 340 `(anonymous namespace)::Actuator::~Actuator()`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 172 `collapse_setup`
- 168 `_free_r`
- 156 `sub0::detail::Broker<(anonymous namespace)::Sample>::unsubscribe(sub0::Subscribe<(anonymous namespace)::Sample>*) [clone .constprop.0]`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 48 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&) [clone .isra.0]`
- 36 `collapse_publish`
- 20 `collapse::work(unsigned long)`
- 12 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b2_static (bytes)</summary>

- 72 `void sub0x::StaticWiring<&(anonymous namespace)::relay, &(anonymous namespace)::actuator, &(anonymous namespace)::tail>::publish<(anonymous namespace)::Sample>((anonymous namespace)::Sample const&)`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 500 `(anonymous namespace)::Actuator::~Actuator()`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 132 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::publish((anonymous namespace)::Sample const&, void const*, sub0x::PublishReport*) const [clone .constprop.0]`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 388 `(anonymous namespace)::Actuator::~Actuator()`
- 256 `_malloc_r`
- 254 `memmove`
- 172 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 168 `_free_r`
- 124 `collapse_setup`
- 104 `(anonymous namespace)::Relay::receive((anonymous namespace)::Sample const&)`
- 76 `_impure_data`

</details>

## Case: one_receiver

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 15.0 (+0.0) | 19 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 31.0 (+16.0) | 25 (+6) | 16 (+0) | 17 (+6) | 1/1 | 2515 (+164) | 648 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 16.0 (+1.0) | 21 (+2) | 16 (+0) | 12 (+1) | 0/0 | 2367 (+16) | 632 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 47.0 (+32.0) | 34 (+15) | 49 (+33) | 51 (+40) | 2/0 | 5048 (+2697) | 992 (+368) | 248/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 47.0 (+32.0) | 34 (+15) | 49 (+33) | 51 (+40) | 2/0 | 5048 (+2697) | 992 (+368) | 248/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 26.0 (+11.0) | 34 (+15) | 49 (+33) | 23 (+12) | 0/0 | 4812 (+2461) | 984 (+360) | 248/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 16.0 (+0.0) | 21 (+0) | 16 (+0) | 12 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 15.0 (+0.0) | 19 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 31.0 (+0.0) | 25 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2515 (+0) | 648 (+0) | 0/34 | - | PASS |
| sub0x_dynamic | ok | 53.0 (+38.0) | 38 (+19) | 66 (+50) | 57 (+46) | 2/0 | 4576 (+2225) | 936 (+312) | 0/713 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 26.0 (+11.0) | 38 (+19) | 53 (+37) | 23 (+12) | 0/0 | 4052 (+1701) | 904 (+280) | 0/545 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 19 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 25 (+6) | 16 (+0) | 17 (+13) | 1/1 | 2483 (+164) | 648 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 21 (+2) | 16 (+0) | 4 (+0) | 0/0 | 2335 (+16) | 632 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 21 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2335 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 19 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 624 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 25 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2483 (+0) | 648 (+0) | 0/5 | - | PASS |
| sub0x_dynamic | ok | 8.0 (+0.0) | 38 (+19) | 53 (+37) | 4 (+0) | 0/0 | 4048 (+1729) | 920 (+296) | 0/563 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 8.0 (+0.0) | 38 (+19) | 53 (+37) | 4 (+0) | 0/0 | 3956 (+1637) | 904 (+280) | 0/545 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 79 `collapse_publish`
- 57 `collapse_setup`
- 34 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::node`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 43 `collapse_publish`
- 29 `collapse_setup`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 862 `(anonymous namespace)::Controller::~Controller()`
- 304 `collapse_teardown`
- 185 `collapse_publish`
- 103 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 862 `(anonymous namespace)::Controller::~Controller()`
- 304 `collapse_teardown`
- 185 `collapse_publish`
- 103 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 862 `(anonymous namespace)::Controller::~Controller()`
- 304 `collapse_teardown`
- 103 `collapse_setup`
- 83 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 34 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 423 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 222 `collapse_publish`
- 113 `collapse_setup`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 113 `collapse_setup`
- 83 `collapse_publish`
- 75 `(anonymous namespace)::Controller::~Controller()`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 45 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 26.0 (+8.0) | 21 (+4) | 14 (+0) | 12 (-2) | 0/1 | 2202 (+96) | 680 (+16) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 17.0 (-1.0) | 19 (+2) | 14 (+0) | 13 (-1) | 0/0 | 2122 (+16) | 672 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 78.0 (+60.0) | 32 (+15) | 54 (+40) | 66 (+52) | 1/2 | 4442 (+2336) | 1033 (+369) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 78.0 (+60.0) | 32 (+15) | 54 (+40) | 66 (+52) | 1/2 | 4442 (+2336) | 1033 (+369) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 71.0 (+53.0) | 32 (+15) | 54 (+40) | 55 (+41) | 0/2 | 4286 (+2180) | 1025 (+361) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 17.0 (+0.0) | 19 (+0) | 14 (+0) | 13 (+0) | 0/0 | 2122 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 25.0 (-1.0) | 21 (+0) | 14 (+0) | 22 (+10) | 1/0 | 2214 (+12) | 680 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic | ok | 82.0 (+64.0) | 36 (+19) | 58 (+44) | 70 (+56) | 1/2 | 4316 (+2210) | 936 (+272) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 41.0 (+23.0) | 36 (+19) | 50 (+36) | 28 (+14) | 0/1 | 3724 (+1618) | 904 (+240) | 0/571 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 21 (+5) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+32) | 680 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| sub0pub_spike | ok | 69.0 (+61.0) | 32 (+16) | 54 (+40) | 66 (+62) | 1/2 | 4426 (+2352) | 1033 (+377) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 69.0 (+61.0) | 32 (+16) | 54 (+40) | 66 (+62) | 1/2 | 4426 (+2352) | 1033 (+377) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 62.0 (+54.0) | 32 (+16) | 54 (+40) | 55 (+51) | 0/2 | 4270 (+2196) | 1025 (+369) | 596/0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 21 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+0) | 680 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 73.0 (+65.0) | 36 (+20) | 58 (+44) | 70 (+66) | 1/2 | 4300 (+2226) | 936 (+280) | 0/763 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 32.0 (+24.0) | 36 (+20) | 50 (+36) | 28 (+24) | 0/1 | 3708 (+1634) | 904 (+248) | 0/571 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 39 `collapse_setup`
- 29 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 8 `(anonymous namespace)::node`
- 4 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 25 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 4 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 265 `collapse_publish`
- 104 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 265 `collapse_publish`
- 104 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 300 `sub0::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 219 `collapse_publish`
- 104 `collapse_setup`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 48 `vtable for sub0::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 43 `typeinfo name for sub0::Subscribe<(anonymous namespace)::Sample>`
- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 29 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSA_ENUlPKvRKS2_E_8__invokeESH_SJ_`
- 8 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 482 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 282 `collapse_publish`
- 109 `collapse_setup`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 48 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 48 `vtable for (anonymous namespace)::Controller`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 309 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 109 `collapse_setup`
- 85 `collapse_publish`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::global_`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for (anonymous namespace)::Controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 15 (+0) | 0/0 | 1128 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+0) | 0/1 | 1176 (+48) | 524 (+12) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 17 (+2) | 0/0 | 1140 (+12) | 516 (+4) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 146 (+131) | 2/0 | 2668 (+1540) | 660 (+148) | 64/0 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 146 (+131) | 2/0 | 2668 (+1540) | 660 (+148) | 64/0 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 31 (+16) | 1/0 | 2364 (+1236) | 660 (+148) | 64/0 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 17 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 15 (+0) | 0/0 | 1128 (+0) | 512 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1176 (+0) | 524 (+0) | 0/28 | - | PASS |
| sub0x_dynamic | - | - | - | - | 148 (+133) | 2/0 | 2736 (+1608) | 912 (+400) | 0/68 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 31 (+16) | 1/0 | 2352 (+1224) | 652 (+140) | 0/56 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1104 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1152 (+48) | 524 (+12) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 7 (+0) | 0/0 | 1112 (+8) | 516 (+4) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64/0 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64/0 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64/0 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 7 (+0) | 0/0 | 1112 (+0) | 516 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1104 (+0) | 512 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1152 (+0) | 524 (+0) | 0/2 | - | PASS |
| sub0x_dynamic | - | - | - | - | 7 (+0) | 0/0 | 2372 (+1268) | 912 (+400) | 0/68 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 7 (+0) | 0/0 | 2296 (+1192) | 652 (+140) | 0/56 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 36 `collapse_setup`
- 28 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::sensor`
- 4 `(anonymous namespace)::node`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 44 `collapse_publish`
- 20 `collapse_setup`
- 4 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_spike (bytes)</summary>

- 340 `(anonymous namespace)::Controller::~Controller()`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 76 `collapse_publish`
- 76 `_impure_data`
- 72 `sbrk_aligned`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 340 `(anonymous namespace)::Controller::~Controller()`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 76 `collapse_publish`
- 76 `_impure_data`
- 72 `sbrk_aligned`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 340 `(anonymous namespace)::Controller::~Controller()`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 68 `collapse_setup`
- 52 `collapse_publish`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 28 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 4 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 500 `(anonymous namespace)::Controller::~Controller()`
- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 80 `collapse_publish`
- 76 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 388 `(anonymous namespace)::Controller::~Controller()`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 76 `collapse_setup`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 52 `collapse_publish`

</details>

## Case: publisher_ergonomics

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 52.0 (+22.0) | 31 (+10) | 16 (+0) | 17 (-9) | 1/1 | 2627 (+196) | 672 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 38.0 (+8.0) | 27 (+6) | 16 (+0) | 35 (+9) | 0/0 | 2495 (+64) | 656 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 52.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2627 (+0) | 672 (+0) | 0/99 | - | PASS |
| alt6_static_bound | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin | skipped: g++ lacks deducing-this | | | | | | | | | | |
| alt8_deducing_this_callsite | skipped: g++ lacks deducing-this | | | | | | | | | | |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 24.0 (+15.0) | 31 (+10) | 16 (+0) | 17 (+11) | 1/1 | 2531 (+164) | 672 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 27 (+6) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+32) | 656 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 24.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2531 (+0) | 672 (+0) | 0/11 | - | PASS |
| alt6_static_bound | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin | skipped: g++ lacks deducing-this | | | | | | | | | | |
| alt8_deducing_this_callsite | skipped: g++ lacks deducing-this | | | | | | | | | | |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 105 `collapse_setup`
- 99 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::node`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 114 `collapse_publish`
- 77 `collapse_setup`
- 24 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by alt4_call_site_out (bytes)</summary>

- 24 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by alt5_sink_typeerased (bytes)</summary>

- 99 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::bus`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 45.0 (+12.0) | 27 (+8) | 14 (+0) | 12 (-17) | 0/1 | 2314 (+144) | 704 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | reference; PASS |
| alt1_baseline_template (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 37.0 (+4.0) | 25 (+6) | 14 (+0) | 33 (+4) | 0/0 | 2218 (+48) | 696 (+24) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| alt4_call_site_out (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 44.0 (-1.0) | 27 (+0) | 14 (+0) | 41 (+29) | 1/0 | 2314 (+0) | 704 (+0) | 0/0 | - | FAIL: publish path |
| alt6_static_bound | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 18.0 (+9.0) | 27 (+10) | 14 (+0) | 12 (+8) | 0/1 | 2234 (+160) | 704 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 19 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+16) | 672 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 10.0 (+1.0) | 25 (+6) | 14 (+0) | 7 (+3) | 0/0 | 2154 (+64) | 696 (+24) | 0/0 | - | FAIL: setup instr, publish path, no extra RAM |
| alt4_call_site_out (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 17.0 (-1.0) | 27 (+0) | 14 (+0) | 14 (+2) | 1/0 | 2239 (+5) | 704 (+0) | 0/0 | - | PASS |
| alt6_static_bound | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0/0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0/0 | - | PASS |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 87 `collapse_setup`
- 81 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::logger`
- 4 `(anonymous namespace)::controllerB`
- 4 `(anonymous namespace)::controllerA`

</details>

<details><summary>clang-O2: largest symbols added by alt3_ctad_factory (bytes)</summary>

- 93 `collapse_publish`
- 73 `collapse_setup`
- 24 `(anonymous namespace)::sensor`
- 4 `(anonymous namespace)::logger`
- 4 `(anonymous namespace)::controllerB`
- 4 `(anonymous namespace)::controllerA`

</details>

<details><summary>clang-O2: largest symbols added by alt5_sink_typeerased (bytes)</summary>

- 81 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerES6_NS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 24 `(anonymous namespace)::bus`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-15) | 0/1 | 1240 (+52) | 540 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 32 (+2) | 0/0 | 1200 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1204 (+4) | 532 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1240 (+0) | 540 (+0) | 0/68 | - | PASS |
| alt6_static_bound | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin | skipped: arm-none-eabi-g++ lacks deducing-this | | | | | | | | | | |
| alt8_deducing_this_callsite | skipped: arm-none-eabi-g++ lacks deducing-this | | | | | | | | | | |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+3) | 0/1 | 1184 (+48) | 540 (+20) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1148 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1152 (+4) | 532 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1184 (+0) | 540 (+0) | 0/10 | - | PASS |
| alt6_static_bound | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin | skipped: arm-none-eabi-g++ lacks deducing-this | | | | | | | | | | |
| alt8_deducing_this_callsite | skipped: arm-none-eabi-g++ lacks deducing-this | | | | | | | | | | |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 68 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 60 `collapse_setup`
- 12 `(anonymous namespace)::node`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 44 `collapse_setup`
- 12 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by alt3_ctad_factory (bytes)</summary>

- 48 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by alt4_call_site_out (bytes)</summary>

- 12 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by alt5_sink_typeerased (bytes)</summary>

- 68 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 12 `(anonymous namespace)::bus`

</details>

## Case: static_dynamic_bridge

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 37.0 (+0.0) | 31 (+0) | 32 (+0) | 33 (+0) | 0/0 | 3208 (+0) | 800 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 37.0 (+0.0) | 51 (+20) | 86 (+54) | 33 (+0) | 0/0 | 4669 (+1461) | 944 (+144) | 0/518 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 74.0 (+37.0) | 75 (+44) | 137 (+105) | 36 (+3) | 1/1 | 5382 (+2174) | 1040 (+240) | 0/837 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 37.0 (+0.0) | 31 (+0) | 32 (+0) | 33 (+0) | 0/0 | 3256 (+48) | 800 (+0) | 0/77 | - | FAIL: no Sub0Pub retained |
| sub0x_bridge_slots_cpp23 | ok | 37.0 (+0.0) | 31 (+0) | 32 (+0) | 33 (+0) | 0/0 | 3256 (+48) | 800 (+0) | 0/77 | - | FAIL: no Sub0Pub retained |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 31 (+0) | 32 (+0) | 6 (+0) | 0/0 | 3112 (+0) | 800 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 9.0 (+0.0) | 51 (+20) | 86 (+54) | 6 (+0) | 0/0 | 4573 (+1461) | 944 (+144) | 0/518 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 51.0 (+42.0) | 75 (+44) | 137 (+105) | 36 (+30) | 1/1 | 5318 (+2206) | 1040 (+240) | 0/786 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 9.0 (+0.0) | 31 (+0) | 32 (+0) | 6 (+0) | 0/0 | 3160 (+48) | 800 (+0) | 0/77 | - | FAIL: no Sub0Pub retained |
| sub0x_bridge_slots_cpp23 | ok | 9.0 (+0.0) | 31 (+0) | 32 (+0) | 6 (+0) | 0/0 | 3160 (+48) | 800 (+0) | 0/77 | - | FAIL: no Sub0Pub retained |

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 326 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 218 `collapse_teardown`
- 210 `collapse_setup`
- 80 `(anonymous namespace)::domain`
- 75 `(anonymous namespace)::Probe::~Probe()`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 45 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 326 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 313 `collapse_setup`
- 242 `collapse_teardown`
- 117 `typeinfo name for sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>`
- 117 `collapse_publish`
- 80 `(anonymous namespace)::domain`
- 75 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::~StaticAdapter()`
- 75 `(anonymous namespace)::Probe::~Probe()`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 72 `(anonymous namespace)::port`
- 61 `typeinfo name for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`
- 16 `typeinfo for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_slots_cpp23 (bytes)</summary>

- 72 `(anonymous namespace)::port`
- 61 `typeinfo name for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`
- 16 `typeinfo for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 58.0 (+0.0) | 27 (+0) | 30 (+0) | 44 (+0) | 0/1 | 3078 (+0) | 808 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 57.0 (-1.0) | 40 (+13) | 67 (+37) | 44 (+0) | 0/1 | 4150 (+1072) | 944 (+136) | 0/511 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 68.0 (+10.0) | 57 (+30) | 113 (+83) | 28 (-16) | 0/1 | 4642 (+1564) | 1032 (+224) | 0/762 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 58.0 (+0.0) | 27 (+0) | 30 (+0) | 44 (+0) | 0/1 | 3108 (+30) | 808 (+0) | 0/76 | - | FAIL: no Sub0Pub retained |
| sub0x_bridge_slots_cpp23 | ok | 58.0 (+0.0) | 27 (+0) | 30 (+0) | 44 (+0) | 0/1 | 3108 (+30) | 808 (+0) | 0/76 | - | FAIL: no Sub0Pub retained |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 34.0 (+0.0) | 27 (+0) | 30 (+0) | 29 (+0) | 0/1 | 3021 (+0) | 808 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 33.0 (-1.0) | 40 (+13) | 67 (+37) | 29 (+0) | 0/1 | 4102 (+1081) | 944 (+136) | 0/511 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 42.0 (+8.0) | 57 (+30) | 113 (+83) | 28 (-1) | 0/1 | 4578 (+1557) | 1032 (+224) | 0/716 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 34.0 (+0.0) | 27 (+0) | 30 (+0) | 29 (+0) | 0/1 | 3051 (+30) | 808 (+0) | 0/76 | - | FAIL: no Sub0Pub retained |
| sub0x_bridge_slots_cpp23 | ok | 34.0 (+0.0) | 27 (+0) | 30 (+0) | 29 (+0) | 0/1 | 3051 (+30) | 808 (+0) | 0/76 | - | FAIL: no Sub0Pub retained |

<details><summary>clang-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 321 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 161 `collapse_setup`
- 80 `(anonymous namespace)::domain`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for (anonymous namespace)::Probe`
- 24 `typeinfo for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 321 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 254 `collapse_setup`
- 116 `typeinfo name for sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>`
- 80 `(anonymous namespace)::domain`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 53 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::receive((anonymous namespace)::Sample const&)`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 72 `(anonymous namespace)::port`
- 60 `typeinfo name for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`
- 16 `typeinfo for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_slots_cpp23 (bytes)</summary>

- 72 `(anonymous namespace)::port`
- 60 `typeinfo name for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`
- 16 `typeinfo for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 45 (+0) | 1/0 | 1568 (+0) | 552 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | - | - | - | - | 48 (+3) | 1/0 | 3576 (+2008) | 668 (+116) | 0/20 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | - | - | - | - | 24 (-21) | 0/1 | 3720 (+2152) | 680 (+128) | 0/444 | operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | - | - | - | - | 45 (+0) | 1/0 | 1568 (+0) | 552 (+0) | 0/0 | - | PASS |
| sub0x_bridge_slots_cpp23 | - | - | - | - | 45 (+0) | 1/0 | 1568 (+0) | 552 (+0) | 0/0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1488 (+0) | 552 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | - | - | - | - | 12 (+0) | 0/0 | 3488 (+2000) | 668 (+116) | 0/20 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | - | - | - | - | 24 (+12) | 0/1 | 3664 (+2176) | 680 (+128) | 0/408 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | - | - | - | - | 12 (+0) | 0/0 | 1488 (+0) | 552 (+0) | 0/0 | - | PASS |
| sub0x_bridge_slots_cpp23 | - | - | - | - | 12 (+0) | 0/0 | 1488 (+0) | 552 (+0) | 0/0 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 396 `(anonymous namespace)::Probe::~Probe()`
- 256 `_malloc_r`
- 236 `memcpy`
- 168 `_free_r`
- 156 `collapse_setup`
- 128 `collapse_teardown`
- 100 `__sigtramp`
- 96 `collapse_publish`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 256 `_malloc_r`
- 236 `memcpy`
- 168 `_free_r`
- 160 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 124 `collapse_teardown`
- 108 `collapse_setup`
- 100 `__sigtramp`
- 96 `__sigtramp_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 36 `(anonymous namespace)::port`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_slots_cpp23 (bytes)</summary>

- 36 `(anonymous namespace)::port`

</details>

## Case: static_dynamic_bridge_churn

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 81.8 (+0.0) | 42 (+0) | 55 (+0) | 100 (+0) | 4/0 | 3690 (+0) | 832 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 84.3 (+2.5) | 76 (+34) | 137 (+82) | 200 (+100) | 3/0 | 5125 (+1435) | 976 (+144) | 0/518 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 122.8 (+41.0) | 99 (+57) | 192 (+137) | 177 (+77) | 3/2 | 5766 (+2076) | 1072 (+240) | 0/837 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 75.0 (-6.8) | 42 (+0) | 55 (+0) | 115 (+15) | 2/0 | 3758 (+68) | 832 (+0) | 0/77 | - | FAIL: publish path, no Sub0Pub retained |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.8 (+0.0) | 42 (+0) | 55 (+0) | 57 (+0) | 2/0 | 3486 (+0) | 832 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 39.0 (+8.2) | 76 (+34) | 137 (+82) | 136 (+79) | 3/0 | 4885 (+1399) | 976 (+144) | 0/518 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 91.0 (+60.2) | 99 (+57) | 192 (+137) | 177 (+120) | 3/2 | 5702 (+2216) | 1072 (+240) | 0/786 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 30.8 (+0.0) | 42 (+0) | 55 (+0) | 57 (+0) | 2/0 | 3534 (+48) | 832 (+0) | 0/77 | - | FAIL: no Sub0Pub retained |

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 419 `collapse_publish`
- 329 `collapse_setup`
- 326 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 234 `collapse_teardown`
- 80 `(anonymous namespace)::domain`
- 75 `(anonymous namespace)::Probe::~Probe()`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 45 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 457 `collapse_setup`
- 330 `collapse_publish`
- 326 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 258 `collapse_teardown`
- 117 `typeinfo name for sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>`
- 80 `(anonymous namespace)::domain`
- 75 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::~StaticAdapter()`
- 75 `(anonymous namespace)::Probe::~Probe()`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 443 `collapse_publish`
- 72 `(anonymous namespace)::port`
- 61 `typeinfo name for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`
- 16 `typeinfo for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 94.0 (+0.0) | 32 (+0) | 53 (+0) | 175 (+0) | 0/2 | 3946 (+0) | 832 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 101.0 (+7.0) | 58 (+26) | 113 (+60) | 204 (+29) | 2/2 | 4522 (+576) | 968 (+136) | 0/511 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 111.8 (+17.8) | 73 (+41) | 164 (+111) | 172 (-3) | 2/2 | 4950 (+1004) | 1056 (+224) | 0/762 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 94.0 (+0.0) | 32 (+0) | 53 (+0) | 175 (+0) | 0/2 | 3976 (+30) | 832 (+0) | 0/76 | - | FAIL: no Sub0Pub retained |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 58.5 (+0.0) | 32 (+0) | 53 (+0) | 144 (+0) | 0/2 | 3825 (+0) | 832 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | 65.8 (+7.3) | 58 (+26) | 113 (+60) | 174 (+30) | 2/2 | 4410 (+585) | 968 (+136) | 0/511 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | 74.5 (+16.0) | 73 (+41) | 164 (+111) | 172 (+28) | 2/2 | 4886 (+1061) | 1056 (+224) | 0/716 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | 58.5 (+0.0) | 32 (+0) | 53 (+0) | 144 (+0) | 0/2 | 3855 (+30) | 832 (+0) | 0/76 | - | FAIL: no Sub0Pub retained |

<details><summary>clang-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 321 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 267 `collapse_setup`
- 80 `(anonymous namespace)::domain`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for (anonymous namespace)::Probe`
- 24 `typeinfo for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 351 `collapse_setup`
- 321 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 116 `typeinfo name for sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>`
- 80 `(anonymous namespace)::domain`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 53 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::receive((anonymous namespace)::Sample const&)`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`
- 40 `vtable for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 72 `(anonymous namespace)::port`
- 60 `typeinfo name for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`
- 16 `typeinfo for sub0x::DynamicPort<(anonymous namespace)::Sample, 8u>::Receiver`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 187 (+0) | 5/0 | 1688 (+0) | 564 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | - | - | - | - | 280 (+93) | 6/0 | 3688 (+2000) | 688 (+124) | 0/196 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | - | - | - | - | 263 (+76) | 7/1 | 3808 (+2120) | 700 (+136) | 0/480 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | - | - | - | - | 187 (+0) | 5/0 | 1688 (+0) | 564 (+0) | 0/134 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 142 (+0) | 2/0 | 1576 (+0) | 564 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | - | - | - | - | 237 (+95) | 3/0 | 3584 (+2008) | 688 (+124) | 0/108 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | - | - | - | - | 263 (+121) | 7/1 | 3752 (+2176) | 700 (+136) | 0/444 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | - | - | - | - | 142 (+0) | 2/0 | 1576 (+0) | 564 (+0) | 0/54 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 396 `(anonymous namespace)::Probe::~Probe()`
- 256 `_malloc_r`
- 236 `memcpy`
- 168 `_free_r`
- 136 `collapse_teardown`
- 108 `collapse_setup`
- 100 `__sigtramp`
- 96 `__sigtramp_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 256 `_malloc_r`
- 236 `memcpy`
- 168 `_free_r`
- 160 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 132 `collapse_teardown`
- 128 `collapse_setup`
- 100 `__sigtramp`
- 96 `__sigtramp_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 80 `void sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger, &(anonymous namespace)::port>::publish<(anonymous namespace)::Sample>((anonymous namespace)::Sample const&)`
- 54 `sub0x::DynamicPort<(anonymous namespace)::Sample, 8ul>::remove(sub0x::DynamicPort<(anonymous namespace)::Sample, 8ul>::Receiver*)`
- 36 `(anonymous namespace)::port`

</details>

## Case: static_dynamic_bridge_empty

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 23.0 (+0.0) | 19 (+0) | 16 (+0) | 19 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | 26.0 (+3.0) | 25 (+6) | 16 (+0) | 22 (+3) | 0/0 | 2576 (+209) | 720 (+96) | 0/0 | pure virtual | reference; FAIL: publish instr, setup instr, publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 27.0 (+1.0) | 28 (+3) | 35 (+19) | 24 (+2) | 0/0 | 3041 (+465) | 744 (+24) | 0/0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 33.0 (+7.0) | 52 (+27) | 86 (+70) | 29 (+7) | 0/0 | 4778 (+2202) | 944 (+224) | 0/837 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 26.0 (+0.0) | 25 (+0) | 16 (+0) | 22 (+0) | 0/0 | 2576 (+0) | 720 (+0) | 0/0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 19 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2335 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | 12.0 (+3.0) | 25 (+6) | 16 (+0) | 9 (+3) | 0/0 | 2544 (+209) | 720 (+96) | 0/0 | pure virtual | reference; FAIL: publish instr, setup instr, publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 13.0 (+1.0) | 28 (+3) | 35 (+19) | 10 (+1) | 0/0 | 2993 (+449) | 744 (+24) | 0/0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 20.0 (+8.0) | 52 (+27) | 86 (+70) | 23 (+14) | 0/0 | 4714 (+2170) | 944 (+224) | 0/786 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 12.0 (+0.0) | 25 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2544 (+0) | 720 (+0) | 0/0 | - | PASS |

<details><summary>gcc-O2: largest symbols added by handwritten_registry (bytes)</summary>

- 77 `collapse_publish`
- 72 `(anonymous namespace)::registry`
- 58 `collapse_setup`
- 6 `collapse_publish.cold`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 186 `collapse_teardown`
- 81 `collapse_publish`
- 80 `(anonymous namespace)::domain`
- 76 `collapse_setup`
- 8 `(anonymous namespace)::port`
- 5 `collapse_teardown.cold`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 326 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 218 `collapse_teardown`
- 198 `collapse_setup`
- 117 `typeinfo name for sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>`
- 93 `collapse_publish`
- 80 `(anonymous namespace)::domain`
- 75 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::~StaticAdapter()`
- 67 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 72 `(anonymous namespace)::port`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 24.0 (+0.0) | 17 (+0) | 14 (+0) | 20 (+0) | 0/0 | 2122 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | 35.0 (+11.0) | 23 (+6) | 14 (+0) | 44 (+24) | 0/1 | 2278 (+156) | 736 (+72) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 35.0 (+0.0) | 26 (+3) | 26 (+12) | 44 (+0) | 0/1 | 2592 (+314) | 760 (+24) | 0/0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 50.0 (+15.0) | 40 (+17) | 67 (+53) | 28 (-16) | 0/1 | 4227 (+1949) | 944 (+208) | 0/762 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 35.0 (+0.0) | 23 (+0) | 14 (+0) | 44 (+0) | 0/1 | 2278 (+0) | 736 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | 20.0 (+11.0) | 23 (+6) | 14 (+0) | 29 (+25) | 0/1 | 2246 (+172) | 736 (+72) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 20.0 (+0.0) | 26 (+3) | 26 (+12) | 29 (+0) | 0/1 | 2560 (+314) | 760 (+24) | 0/0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 33.0 (+13.0) | 40 (+17) | 67 (+53) | 28 (-1) | 0/1 | 4179 (+1933) | 944 (+208) | 0/716 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 20.0 (+0.0) | 23 (+0) | 14 (+0) | 29 (+0) | 0/1 | 2246 (+0) | 736 (+0) | 0/0 | - | PASS |

<details><summary>clang-O2: largest symbols added by handwritten_registry (bytes)</summary>

- 135 `collapse_publish`
- 72 `(anonymous namespace)::registry`
- 53 `collapse_setup`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 219 `collapse_teardown`
- 80 `(anonymous namespace)::domain`
- 70 `collapse_setup`
- 8 `_ZN12_GLOBAL__N_14portE.0`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 321 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 235 `collapse_teardown`
- 162 `collapse_setup`
- 116 `typeinfo name for sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>`
- 80 `(anonymous namespace)::domain`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 53 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::receive((anonymous namespace)::Sample const&)`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 72 `(anonymous namespace)::port`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 22 (+0) | 0/0 | 1144 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_registry | - | - | - | - | 28 (+6) | 0/0 | 1180 (+36) | 548 (+36) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | - | - | - | - | 28 (+0) | 0/0 | 2716 (+1536) | 656 (+108) | 0/0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | - | - | - | - | 42 (+14) | 1/0 | 3564 (+2384) | 668 (+120) | 0/494 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | - | - | - | - | 28 (+0) | 0/0 | 1180 (+0) | 548 (+0) | 0/0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 512 (+0) | 0/0 | - | reference |
| handwritten_registry | - | - | - | - | 18 (+6) | 0/0 | 1152 (+36) | 548 (+36) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | - | - | - | - | 18 (+0) | 0/0 | 2688 (+1536) | 656 (+108) | 0/0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | - | - | - | - | 21 (+3) | 0/0 | 3520 (+2368) | 668 (+120) | 0/452 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | - | - | - | - | 18 (+0) | 0/0 | 1152 (+0) | 548 (+0) | 0/0 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_registry (bytes)</summary>

- 72 `collapse_publish`
- 36 `(anonymous namespace)::registry`
- 32 `collapse_setup`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 256 `_malloc_r`
- 236 `memcpy`
- 116 `collapse_teardown`
- 100 `__sigtramp`
- 96 `__sigtramp_r`
- 84 `raise`
- 80 `_raise_r`
- 76 `signal`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 396 `sub0x::StaticAdapter<sub0x::StaticWiring<&(anonymous namespace)::controller, &(anonymous namespace)::logger>, (anonymous namespace)::Sample>::~StaticAdapter()`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 156 `collapse_setup`
- 128 `collapse_teardown`
- 100 `__sigtramp`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 36 `(anonymous namespace)::port`

</details>

## Case: transport_endpoint

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 45.0 (+18.0) | 26 (+8) | 16 (+0) | 26 (+3) | 1/1 | 2563 (+180) | 656 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 27.0 (+0.0) | 22 (+4) | 16 (+0) | 23 (+0) | 0/0 | 2415 (+32) | 640 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 27.0 (+0.0) | 22 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2415 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | 27.0 (+0.0) | 22 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2415 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 45.0 (+0.0) | 26 (+0) | 16 (+0) | 26 (+0) | 1/1 | 2563 (+0) | 656 (+0) | 0/52 | - | PASS |
| sub0x_dynamic_route | ok | 205.0 (+178.0) | 62 (+44) | 124 (+108) | 115 (+92) | 3/2 | 5973 (+3590) | 1056 (+440) | 0/1223 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 143.0 (+116.0) | 62 (+44) | 124 (+108) | 76 (+53) | 1/2 | 5521 (+3138) | 1016 (+400) | 0/1173 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 28.0 (+14.0) | 26 (+8) | 16 (+0) | 17 (+6) | 1/1 | 2499 (+148) | 656 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 14.0 (+0.0) | 22 (+4) | 16 (+0) | 11 (+0) | 0/0 | 2383 (+32) | 640 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 14.0 (+0.0) | 22 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2383 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | 14.0 (+0.0) | 22 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2383 (+0) | 640 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 28.0 (+0.0) | 26 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2499 (+0) | 656 (+0) | 0/26 | - | PASS |
| sub0x_dynamic_route | ok | 189.0 (+175.0) | 62 (+44) | 124 (+108) | 115 (+104) | 3/2 | 5957 (+3606) | 1056 (+440) | 0/1223 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 127.0 (+113.0) | 62 (+44) | 124 (+108) | 76 (+65) | 1/2 | 5505 (+3154) | 1016 (+400) | 0/1173 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 110 `collapse_publish`
- 61 `collapse_setup`
- 52 `(anonymous namespace)::deliverEgress(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::sensor`
- 16 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 33 `collapse_setup`
- 16 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 16 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire_origin_transport (bytes)</summary>

- 16 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 52 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, sub0x::Forward<(anonymous namespace)::Radio> >, 0>(sub0x::Wiring<(anonymous namespace)::Controller, sub0x::Forward<(anonymous namespace)::Radio> >&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_route (bytes)</summary>

- 526 `collapse_publish`
- 409 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .part.0]`
- 294 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::~Route()`
- 278 `collapse_setup`
- 147 `(anonymous namespace)::Controller::~Controller()`
- 146 `collapse_teardown`
- 102 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::receive((anonymous namespace)::Sample const&)`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_route_lean (bytes)</summary>

- 385 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .part.0]`
- 336 `collapse_publish`
- 294 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::~Route()`
- 278 `collapse_setup`
- 147 `(anonymous namespace)::Controller::~Controller()`
- 146 `collapse_teardown`
- 102 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::receive((anonymous namespace)::Sample const&)`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)1>, sub0x::NoFilter> >::global_`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 43.0 (+14.0) | 22 (+6) | 14 (+0) | 23 (-2) | 0/1 | 2258 (+120) | 688 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 43.0 (+0.0) | 22 (+0) | 14 (+0) | 40 (+17) | 1/0 | 2271 (+13) | 688 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic_route | ok | 246.0 (+217.0) | 60 (+44) | 121 (+107) | 139 (+114) | 2/4 | 5425 (+3287) | 1048 (+392) | 0/1056 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 141.0 (+112.0) | 60 (+44) | 121 (+107) | 74 (+49) | 0/2 | 4917 (+2779) | 1016 (+360) | 0/1035 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 22.0 (+8.0) | 22 (+6) | 14 (+0) | 11 (+1) | 0/1 | 2202 (+112) | 688 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0/0 | - | reference; PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 22 (+0) | 14 (+0) | 19 (+8) | 1/0 | 2206 (+4) | 688 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic_route | ok | 228.0 (+214.0) | 60 (+44) | 121 (+107) | 139 (+129) | 2/4 | 5409 (+3319) | 1048 (+392) | 0/1056 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 123.0 (+109.0) | 60 (+44) | 121 (+107) | 74 (+64) | 0/2 | 4901 (+2811) | 1016 (+360) | 0/1035 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 46 `(anonymous namespace)::deliverEgress(void const*, (anonymous namespace)::Sample const&)`
- 43 `collapse_setup`
- 16 `(anonymous namespace)::node`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 46 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerENS_7ForwardINS1_5RadioEEEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSD_ENUlPKvRKS2_E_8__invokeESK_SM_`
- 16 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_route (bytes)</summary>

- 560 `collapse_publish`
- 480 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 286 `collapse_setup`
- 87 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::receive((anonymous namespace)::Sample const&)`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 67 `collapse_teardown`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`
- 64 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::~Route()`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_route_lean (bytes)</summary>

- 480 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 286 `collapse_setup`
- 285 `collapse_publish`
- 85 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::receive((anonymous namespace)::Sample const&)`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)1>, sub0x::NoFilter> >::global_`
- 67 `collapse_teardown`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 64 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::~Route()`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 25 (+0) | 0/1 | 1224 (+76) | 528 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 25 (+0) | 0/0 | 1168 (+20) | 520 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 25 (+0) | 0/0 | 1168 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | - | - | - | - | 25 (+0) | 0/0 | 1168 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 25 (+0) | 0/1 | 1224 (+0) | 528 (+0) | 0/44 | - | PASS |
| sub0x_dynamic_route | - | - | - | - | 20 (-5) | 0/2 | 3056 (+1908) | 928 (+420) | 0/656 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | - | - | - | - | 20 (-5) | 0/2 | 2696 (+1548) | 672 (+164) | 0/588 | operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+2) | 0/1 | 1172 (+60) | 528 (+20) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 13 (+0) | 0/0 | 1132 (+20) | 520 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 13 (+0) | 0/0 | 1132 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | - | - | - | - | 13 (+0) | 0/0 | 1132 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1172 (+0) | 528 (+0) | 0/20 | - | PASS |
| sub0x_dynamic_route | - | - | - | - | 20 (+7) | 0/2 | 3036 (+1924) | 928 (+420) | 0/656 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | - | - | - | - | 20 (+7) | 0/2 | 2676 (+1564) | 672 (+164) | 0/588 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 44 `(anonymous namespace)::deliverEgress(void const*, (anonymous namespace)::Sample const&)`
- 40 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 24 `collapse_setup`
- 8 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 8 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire_origin_transport (bytes)</summary>

- 8 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 44 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, sub0x::Forward<(anonymous namespace)::Radio> >, 0>(sub0x::Wiring<(anonymous namespace)::Controller, sub0x::Forward<(anonymous namespace)::Radio> >&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_route (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 228 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 168 `_free_r`
- 132 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::publish((anonymous namespace)::Sample const&, void const*, sub0x::PublishReport*) const [clone .constprop.0]`
- 92 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::~Route()`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_route_lean (bytes)</summary>

- 256 `_malloc_r`
- 254 `memmove`
- 224 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect()`
- 168 `_free_r`
- 92 `sub0x::Route<(anonymous namespace)::Sample, (anonymous namespace)::RadioPort>::~Route()`
- 88 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)1>, sub0x::NoFilter> >::publish((anonymous namespace)::Sample const&, void const*, sub0x::PublishReport*) const [clone .constprop.0]`
- 80 `collapse_setup`
- 76 `_impure_data`

</details>

## Case: transport_two_links

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 40.0 (+0.0) | 20 (+0) | 16 (+0) | 37 (+0) | 0/0 | 2447 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 45.0 (+5.0) | 26 (+6) | 16 (+0) | 41 (+4) | 0/0 | 2511 (+64) | 656 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 53.0 (+8.0) | 26 (+0) | 16 (+0) | 56 (+15) | 0/0 | 2559 (+48) | 656 (+0) | 0/0 | - | FAIL: publish instr, publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | 45.0 (+0.0) | 26 (+0) | 16 (+0) | 41 (+0) | 0/0 | 2511 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 40.0 (+0.0) | 20 (+0) | 16 (+0) | 37 (+0) | 0/0 | 2447 (+0) | 624 (+0) | 0/0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2399 (+0) | 624 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 31.0 (+4.0) | 26 (+6) | 16 (+0) | 27 (+4) | 0/0 | 2463 (+64) | 656 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 39.0 (+8.0) | 26 (+0) | 16 (+0) | 42 (+15) | 0/0 | 2511 (+48) | 656 (+0) | 0/0 | - | FAIL: publish instr, publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | 31.0 (+0.0) | 26 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2463 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2399 (+0) | 624 (+0) | 0/0 | - | PASS |

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 135 `collapse_publish`
- 67 `collapse_setup`
- 24 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 186 `collapse_publish`
- 24 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire_typed_links (bytes)</summary>

- 24 `(anonymous namespace)::bus`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 40.0 (+0.0) | 18 (+0) | 14 (+0) | 36 (+0) | 0/0 | 2186 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 40.0 (+0.0) | 18 (+0) | 14 (+0) | 36 (+0) | 0/0 | 2186 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 50.0 (+10.0) | 24 (+6) | 14 (+0) | 53 (+17) | 0/0 | 2282 (+96) | 696 (+32) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | 45.0 (+5.0) | 24 (+6) | 14 (+0) | 42 (+6) | 0/0 | 2250 (+64) | 696 (+32) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 40.0 (+0.0) | 18 (+0) | 14 (+0) | 36 (+0) | 0/0 | 2186 (+0) | 664 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 18 (+0) | 14 (+0) | 24 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | 27.0 (+0.0) | 18 (+0) | 14 (+0) | 24 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | reference; PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 36.0 (+9.0) | 24 (+6) | 14 (+0) | 39 (+15) | 0/0 | 2234 (+96) | 696 (+32) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | 31.0 (+4.0) | 24 (+6) | 14 (+0) | 27 (+3) | 0/0 | 2186 (+48) | 696 (+32) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 27.0 (+0.0) | 18 (+0) | 14 (+0) | 24 (+0) | 0/0 | 2138 (+0) | 664 (+0) | 0/0 | - | PASS |

<details><summary>clang-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 163 `collapse_publish`
- 63 `collapse_setup`
- 24 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::radioB`
- 4 `(anonymous namespace)::radioA`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b1_wire_typed_links (bytes)</summary>

- 129 `collapse_publish`
- 63 `collapse_setup`
- 24 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::radioB`
- 4 `(anonymous namespace)::radioA`
- 1 `(anonymous namespace)::controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 37 (+0) | 0/0 | 1200 (+0) | 516 (+0) | 0/0 | - | reference |
| handwritten_runtime | - | - | - | - | 42 (+5) | 0/0 | 1224 (+24) | 532 (+16) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 52 (+10) | 4/0 | 1252 (+28) | 532 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | - | - | - | - | 42 (+0) | 3/0 | 1228 (+4) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 37 (+0) | 0/0 | 1200 (+0) | 516 (+0) | 0/0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 28 (+0) | 0/0 | 1172 (+0) | 516 (+0) | 0/0 | - | reference |
| handwritten_runtime | - | - | - | - | 31 (+3) | 0/0 | 1192 (+20) | 532 (+16) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 41 (+10) | 4/0 | 1220 (+28) | 532 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | - | - | - | - | 29 (-2) | 3/0 | 1192 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 28 (+0) | 0/0 | 1172 (+0) | 516 (+0) | 0/0 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 108 `collapse_publish`
- 40 `collapse_setup`
- 12 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 112 `collapse_publish`
- 24 `_ZNK5sub0x7ForwardIN12_GLOBAL__N_15RadioEE7receiveINS1_6SampleEEEDTcmcldtclL_ZSt7declvalIRS2_EDTcl9__declvalIT_ELi0EEEvEEL_ZNS2_4sendERKS5_Efp_Ecvv_EERKS8_.isra.0`
- 12 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire_typed_links (bytes)</summary>

- 24 `_ZNK5sub0x7ForwardIN12_GLOBAL__N_15RadioEE7receiveINS1_6SampleEEEDTcmcldtclL_ZSt7declvalIRS2_EDTcl9__declvalIT_ELi0EEEvEEL_ZNS2_4sendERKS5_Efp_Ecvv_EERKS8_.isra.0`
- 12 `(anonymous namespace)::bus`

</details>

## Case: two_domains

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 31.0 (+0.0) | 21 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 65.0 (+34.0) | 35 (+14) | 16 (+0) | 29 (+2) | 1/2 | 2767 (+336) | 696 (+64) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | 37.0 (+6.0) | 27 (+6) | 16 (+0) | 32 (+5) | 0/0 | 2479 (+48) | 656 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| handwritten_runtime | ok | 38.0 (+7.0) | 27 (+6) | 16 (+0) | 34 (+7) | 0/0 | 2495 (+64) | 664 (+32) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | 37.0 (+0.0) | 27 (+0) | 16 (+0) | 32 (+0) | 0/0 | 2479 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 34 (+0) | 0/0 | 2495 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | 30.0 (-1.0) | 21 (+0) | 16 (+0) | 26 (-1) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 31.0 (+0.0) | 21 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 65.0 (+0.0) | 35 (+0) | 16 (+0) | 29 (+0) | 1/2 | 2767 (+0) | 696 (+0) | 0/102 | - | PASS |
| sub0x_dynamic_domain | ok | 178.0 (+147.0) | 107 (+86) | 233 (+217) | 128 (+101) | 3/2 | 6286 (+3855) | 1192 (+560) | 0/767 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 102.0 (+71.0) | 107 (+86) | 222 (+206) | 59 (+32) | 1/2 | 5670 (+3239) | 1160 (+528) | 0/664 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 10.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 38.0 (+28.0) | 35 (+14) | 16 (+0) | 29 (+23) | 1/2 | 2671 (+304) | 696 (+64) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | 10.0 (+0.0) | 27 (+6) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+32) | 656 (+24) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_runtime | ok | 11.0 (+1.0) | 27 (+6) | 16 (+0) | 7 (+1) | 0/0 | 2399 (+32) | 664 (+32) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 11.0 (+0.0) | 27 (+0) | 16 (+0) | 7 (+0) | 0/0 | 2399 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | 9.0 (-1.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 10.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 38.0 (+0.0) | 35 (+0) | 16 (+0) | 29 (+0) | 1/2 | 2671 (+0) | 696 (+0) | 0/16 | - | PASS |
| sub0x_dynamic_domain | ok | 153.0 (+143.0) | 107 (+86) | 233 (+217) | 128 (+122) | 3/2 | 6238 (+3871) | 1192 (+560) | 0/767 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 77.0 (+67.0) | 107 (+86) | 222 (+206) | 59 (+53) | 1/2 | 5622 (+3255) | 1160 (+528) | 0/664 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 133 `collapse_setup`
- 118 `collapse_publish`
- 68 `(anonymous namespace)::deliverA(void const*, (anonymous namespace)::Sample const&)`
- 34 `(anonymous namespace)::deliverB(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::sensorB`
- 16 `(anonymous namespace)::sensorA`
- 16 `(anonymous namespace)::nodeA`
- 8 `(anonymous namespace)::nodeB`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_gateway (bytes)</summary>

- 112 `collapse_publish`
- 77 `collapse_setup`
- 24 `(anonymous namespace)::gateway`

</details>

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 119 `collapse_publish`
- 77 `collapse_setup`
- 16 `(anonymous namespace)::sensorA`
- 8 `(anonymous namespace)::sensorB`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 68 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 34 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::busA`
- 8 `(anonymous namespace)::busB`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_domain (bytes)</summary>

- 579 `collapse_publish`
- 517 `collapse_setup`
- 281 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 187 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::Scoped> >::close(sub0x::detail::Table<(anonymous namespace)::Sample, sub0x::config<sub0x::Scoped> >&)`
- 154 `collapse_teardown`
- 81 `void sub0x::kit::forgetInOwnDispatches<(anonymous namespace)::Sample>(void const*, sub0x::Subscribe<(anonymous namespace)::Sample> const*)`
- 80 `(anonymous namespace)::domainB`
- 80 `(anonymous namespace)::domainA`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic_domain_lean (bytes)</summary>

- 517 `collapse_setup`
- 326 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 207 `collapse_publish`
- 154 `collapse_teardown`
- 146 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::Scoped, sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >::close(sub0x::detail::Table<(anonymous namespace)::Sample, sub0x::config<sub0x::Scoped, sub0x::DispatchWith<(sub0x::Dispatch)1>, sub0x::ContextWith<(sub0x::Context)2>, sub0x::NoFilter> >&)`
- 80 `(anonymous namespace)::domainB`
- 80 `(anonymous namespace)::domainA`
- 75 `(anonymous namespace)::Logger::~Logger()`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 58.0 (+24.0) | 29 (+10) | 14 (+0) | 22 (-9) | 0/2 | 2406 (+220) | 712 (+40) | 0/0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0/0 | - | reference; PASS |
| handwritten_runtime | ok | 34.0 (+0.0) | 21 (+2) | 14 (+0) | 31 (+0) | 0/0 | 2202 (+16) | 680 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | 37.0 (+3.0) | 25 (+6) | 14 (+0) | 34 (+3) | 0/0 | 2234 (+48) | 696 (+24) | 0/0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 34.0 (+0.0) | 21 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2202 (+0) | 680 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 58.0 (+0.0) | 29 (+0) | 14 (+0) | 56 (+34) | 2/0 | 2418 (+12) | 712 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_dynamic_domain | ok | 227.0 (+193.0) | 79 (+60) | 195 (+181) | 89 (+58) | 3/2 | 5740 (+3554) | 1176 (+504) | 0/1139 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 97.0 (+63.0) | 79 (+60) | 164 (+150) | 53 (+22) | 0/2 | 4956 (+2770) | 1136 (+464) | 0/511 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 21.0 (+12.0) | 29 (+12) | 14 (+0) | 14 (+8) | 0/1 | 2258 (+168) | 712 (+48) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | 9.0 (+0.0) | 19 (+2) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+16) | 672 (+8) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 18 (+1) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+16) | 664 (+0) | 0/0 | - | reference; FAIL: setup instr |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 9.0 (+0.0) | 18 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 20.0 (-1.0) | 29 (+0) | 14 (+0) | 16 (+2) | 1/0 | 2263 (+5) | 712 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_domain | ok | 199.0 (+190.0) | 79 (+62) | 195 (+181) | 89 (+83) | 3/2 | 5708 (+3618) | 1176 (+512) | 0/1139 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 69.0 (+60.0) | 79 (+62) | 164 (+150) | 53 (+47) | 0/2 | 4924 (+2834) | 1136 (+472) | 0/511 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 101 `collapse_setup`
- 59 `(anonymous namespace)::deliverA(void const*, (anonymous namespace)::Sample const&)`
- 29 `(anonymous namespace)::deliverB(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::nodeA`
- 8 `_ZN12_GLOBAL__N_17sensorBE.0`
- 8 `_ZN12_GLOBAL__N_17sensorAE.0`
- 8 `(anonymous namespace)::nodeB`
- 4 `(anonymous namespace)::loggerA`

</details>

<details><summary>clang-O2: largest symbols added by handwritten_gateway (bytes)</summary>

- 100 `collapse_publish`

</details>

<details><summary>clang-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 42 `collapse_setup`
- 8 `(anonymous namespace)::sensorB`
- 4 `(anonymous namespace)::controllerB`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b1_one_publisher (bytes)</summary>

- 73 `collapse_setup`
- 24 `(anonymous namespace)::gateway`
- 4 `(anonymous namespace)::loggerA`
- 4 `(anonymous namespace)::controllerB`
- 4 `(anonymous namespace)::controllerA`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b2_one_publisher (bytes)</summary>

- 100 `collapse_publish`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 59 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerENS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 29 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSA_ENUlPKvRKS2_E_8__invokeESH_SJ_`
- 16 `(anonymous namespace)::busA`
- 8 `(anonymous namespace)::busB`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_domain (bytes)</summary>

- 490 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 440 `collapse::Slot<sub0x::Domain<(anonymous namespace)::Sample> >::reset()`
- 409 `collapse_setup`
- 278 `(anonymous namespace)::Sensor::send(unsigned int)`
- 84 `collapse_teardown`
- 80 `(anonymous namespace)::domainB`
- 80 `(anonymous namespace)::domainA`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, true>`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_domain_lean (bytes)</summary>

- 411 `collapse_teardown`
- 409 `collapse_setup`
- 321 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 185 `collapse_publish`
- 80 `(anonymous namespace)::domainB`
- 80 `(anonymous namespace)::domainA`
- 66 `typeinfo name for sub0x::detail::SubscriberInterface<(anonymous namespace)::Sample, false>`
- 44 `typeinfo name for sub0x::Subscribe<(anonymous namespace)::Sample>`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 27 (-3) | 0/2 | 1300 (+112) | 548 (+28) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_gateway | - | - | - | - | 32 (+2) | 0/0 | 1200 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| handwritten_runtime | - | - | - | - | 37 (+7) | 0/0 | 1220 (+32) | 532 (+12) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 37 (+0) | 0/0 | 1220 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 27 (+0) | 0/2 | 1300 (+0) | 548 (+0) | 0/80 | - | PASS |
| sub0x_dynamic_domain | - | - | - | - | 23 (-7) | 0/2 | 3760 (+2572) | 1008 (+488) | 0/628 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | - | - | - | - | 39 (+9) | 2/1 | 3828 (+2640) | 748 (+228) | 0/432 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 27 (+15) | 0/2 | 1232 (+96) | 548 (+28) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | - | - | - | - | 12 (+0) | 0/0 | 1148 (+12) | 532 (+12) | 0/0 | - | reference; FAIL: no extra RAM |
| handwritten_runtime | - | - | - | - | 16 (+4) | 0/0 | 1164 (+28) | 532 (+12) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 16 (+0) | 0/0 | 1164 (+0) | 532 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 27 (+0) | 0/2 | 1232 (+0) | 548 (+0) | 0/12 | - | PASS |
| sub0x_dynamic_domain | - | - | - | - | 23 (+11) | 0/2 | 3720 (+2584) | 1008 (+488) | 0/628 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | - | - | - | - | 39 (+27) | 2/1 | 3788 (+2652) | 748 (+228) | 0/432 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 84 `collapse_setup`
- 52 `(anonymous namespace)::deliverA(void const*, (anonymous namespace)::Sample const&)`
- 28 `(anonymous namespace)::deliverB(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::sensorB`
- 8 `(anonymous namespace)::sensorA`
- 8 `(anonymous namespace)::nodeA`
- 4 `(anonymous namespace)::nodeB`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_gateway (bytes)</summary>

- 44 `collapse_setup`
- 12 `(anonymous namespace)::gateway`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 92 `collapse_publish`
- 52 `collapse_setup`
- 8 `(anonymous namespace)::sensorA`
- 4 `(anonymous namespace)::sensorB`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 52 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 28 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::busA`
- 4 `(anonymous namespace)::busB`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_domain (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 254 `memmove`
- 168 `_free_r`
- 160 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 144 `collapse_setup`
- 124 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::config<sub0x::Scoped> >::publish((anonymous namespace)::Sample const&, void const*, sub0x::PublishReport*) const [clone .constprop.0]`
- 100 `__sigtramp`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic_domain_lean (bytes)</summary>

- 256 `_malloc_r`
- 254 `memmove`
- 236 `memcpy`
- 168 `_free_r`
- 160 `sub0x::Subscribe<(anonymous namespace)::Sample>::disconnect() [clone .constprop.0]`
- 144 `collapse_setup`
- 100 `__sigtramp`
- 96 `__sigtramp_r`

</details>

## Case: zero_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 22 (+4) | 16 (+0) | 17 (+13) | 1/1 | 2467 (+148) | 640 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139/0 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139/0 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 21 (+3) | 16 (+0) | 4 (+0) | 0/0 | 3022 (+703) | 728 (+112) | 58/0 | operator delete | FAIL: setup instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 22 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2467 (+0) | 640 (+0) | 0/5 | - | PASS |
| sub0x_dynamic | ok | 27.0 (+19.0) | 18 (+0) | 16 (+0) | 62 (+58) | 4/0 | 2876 (+557) | 728 (+112) | 0/80 | pure virtual | FAIL: publish instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 22 (+4) | 16 (+0) | 17 (+13) | 1/1 | 2467 (+148) | 640 (+24) | 0/0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139/0 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139/0 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 21 (+3) | 16 (+0) | 4 (+0) | 0/0 | 3022 (+703) | 728 (+112) | 58/0 | operator delete | FAIL: setup instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 22 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2467 (+0) | 640 (+0) | 0/5 | - | PASS |
| sub0x_dynamic | ok | 27.0 (+19.0) | 18 (+0) | 16 (+0) | 62 (+58) | 4/0 | 2876 (+557) | 728 (+112) | 0/80 | pure virtual | FAIL: publish instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0/0 | - | PASS |

<details><summary>gcc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 79 `collapse_publish`
- 33 `collapse_setup`
- 16 `(anonymous namespace)::sensor`
- 5 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 1 `(anonymous namespace)::node`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 86 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 42 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for (anonymous namespace)::Sensor`
- 26 `collapse_setup`
- 25 `typeinfo name for (anonymous namespace)::Sensor`
- 24 `typeinfo for (anonymous namespace)::Sensor`
- 24 `(anonymous namespace)::Sensor::~Sensor()`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 86 `collapse_publish`
- 72 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 42 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for (anonymous namespace)::Sensor`
- 26 `collapse_setup`
- 25 `typeinfo name for (anonymous namespace)::Sensor`
- 24 `typeinfo for (anonymous namespace)::Sensor`
- 24 `(anonymous namespace)::Sensor::~Sensor()`

</details>

<details><summary>gcc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 42 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for (anonymous namespace)::Sensor`
- 26 `collapse_setup`
- 25 `typeinfo name for (anonymous namespace)::Sensor`
- 24 `typeinfo for (anonymous namespace)::Sensor`
- 24 `(anonymous namespace)::Sensor::~Sensor()`
- 16 `typeinfo for sub0::Publish<(anonymous namespace)::Sample>`
- 16 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 5 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<>, 0>(sub0x::Wiring<>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 1 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 264 `collapse_publish`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 8 `sub0x::detail::PublishContext<(anonymous namespace)::Sample, (sub0x::Context)0>::top_`
- 5 `collapse_publish.cold`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 18 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+16) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 18 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 41.0 (+33.0) | 16 (+0) | 14 (+0) | 70 (+66) | 1/2 | 2498 (+424) | 744 (+88) | 0/80 | - | FAIL: publish instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained |
| sub0x_dynamic_lean | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 18 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+16) | 0/0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105/0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 18 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | 41.0 (+33.0) | 16 (+0) | 14 (+0) | 70 (+66) | 1/2 | 2498 (+424) | 744 (+88) | 0/80 | - | FAIL: publish instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained |
| sub0x_dynamic_lean | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0/0 | - | PASS |

<details><summary>clang-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 15 `collapse_setup`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 1 `(anonymous namespace)::node`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for (anonymous namespace)::Sensor`
- 26 `collapse_setup`
- 24 `typeinfo name for (anonymous namespace)::Sensor`
- 24 `typeinfo for (anonymous namespace)::Sensor`
- 16 `typeinfo for sub0::Publish<(anonymous namespace)::Sample>`
- 16 `sub0::Publish<(anonymous namespace)::Sample>::~Publish()`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for (anonymous namespace)::Sensor`
- 26 `collapse_setup`
- 24 `typeinfo name for (anonymous namespace)::Sensor`
- 24 `typeinfo for (anonymous namespace)::Sensor`
- 16 `typeinfo for sub0::Publish<(anonymous namespace)::Sample>`
- 16 `sub0::Publish<(anonymous namespace)::Sample>::~Publish()`

</details>

<details><summary>clang-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 41 `typeinfo name for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for sub0::Publish<(anonymous namespace)::Sample>`
- 32 `vtable for (anonymous namespace)::Sensor`
- 26 `collapse_setup`
- 24 `typeinfo name for (anonymous namespace)::Sensor`
- 24 `typeinfo for (anonymous namespace)::Sensor`
- 16 `typeinfo for sub0::Publish<(anonymous namespace)::Sample>`
- 16 `sub0::Publish<(anonymous namespace)::Sample>::~Publish()`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 1 `(anonymous namespace)::bus`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 282 `collapse_publish`
- 72 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 8 `sub0x::detail::PublishContext<(anonymous namespace)::Sample, (sub0x::Context)0>::top_`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1140 (+44) | 520 (+12) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41/0 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41/0 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 1756 (+660) | 612 (+104) | 0/0 | operator delete | FAIL: no extra RAM, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1140 (+0) | 520 (+0) | 0/2 | - | PASS |
| sub0x_dynamic | - | - | - | - | 152 (+145) | 3/0 | 1480 (+384) | 808 (+300) | 0/40 | TLS | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1140 (+44) | 520 (+12) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41/0 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41/0 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 1756 (+660) | 612 (+104) | 0/0 | operator delete | FAIL: no extra RAM, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1140 (+0) | 520 (+0) | 0/2 | - | PASS |
| sub0x_dynamic | - | - | - | - | 152 (+145) | 3/0 | 1480 (+384) | 808 (+300) | 0/40 | TLS | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0/0 | - | PASS |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_erased (bytes)</summary>

- 36 `collapse_publish`
- 24 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 2 `(anonymous namespace)::deliverNode(void const*, (anonymous namespace)::Sample const&)`
- 1 `(anonymous namespace)::node`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_spike (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 68 `collapse_publish`
- 36 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 36 `_sbrk_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual (bytes)</summary>

- 256 `tlsBlock`
- 256 `_malloc_r`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 68 `collapse_publish`
- 36 `sub0::detail::Broker<(anonymous namespace)::Sample>::state_`
- 36 `_sbrk_r`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 256 `_malloc_r`
- 168 `_free_r`
- 76 `_impure_data`
- 72 `sbrk_aligned`
- 36 `_sbrk_r`
- 32 `_sbrk`
- 20 `collapse_setup`
- 18 `(anonymous namespace)::Sensor::~Sensor()`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 2 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<>, 0>(sub0x::Wiring<>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 1 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_dynamic (bytes)</summary>

- 256 `tlsBlock`
- 236 `memcpy`
- 112 `collapse_publish`
- 36 `sub0x::detail::Broker<(anonymous namespace)::Sample, sub0x::Builtin>::global_`
- 4 `sub0x::detail::PublishContext<(anonymous namespace)::Sample, (sub0x::Context)0>::top_`
- 4 `__aeabi_read_tp`

</details>

**Skipped (unsupported feature):** gcc-O2/publisher_ergonomics/observable/alt7_deducing_this_mixin: g++ lacks deducing-this; gcc-O2/publisher_ergonomics/observable/alt8_deducing_this_callsite: g++ lacks deducing-this; gcc-O2/publisher_ergonomics/removable/alt7_deducing_this_mixin: g++ lacks deducing-this; gcc-O2/publisher_ergonomics/removable/alt8_deducing_this_callsite: g++ lacks deducing-this; clang-O2/cancellation/observable/sub0x_alt1c_expected_cpp23: clang++ lacks expected; clang-O2/cancellation/removable/sub0x_alt1c_expected_cpp23: clang++ lacks expected; cm33-gcc-Os/publisher_ergonomics/observable/alt7_deducing_this_mixin: arm-none-eabi-g++ lacks deducing-this; cm33-gcc-Os/publisher_ergonomics/observable/alt8_deducing_this_callsite: arm-none-eabi-g++ lacks deducing-this; cm33-gcc-Os/publisher_ergonomics/removable/alt7_deducing_this_mixin: arm-none-eabi-g++ lacks deducing-this; cm33-gcc-Os/publisher_ergonomics/removable/alt8_deducing_this_callsite: arm-none-eabi-g++ lacks deducing-this


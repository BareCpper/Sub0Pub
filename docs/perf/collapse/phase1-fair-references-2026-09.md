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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0 | - | reference |
| sub0x_alt1_bool | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt2_token | ok | 27.0 (+0.0) | 20 (+0) | 16 (+0) | 29 (+0) | 0/0 | 2431 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt3_static | ok | 29.0 (+2.0) | 20 (+0) | 16 (+0) | 30 (+1) | 0/0 | 2431 (+0) | 632 (+8) | 0 | - | FAIL: no extra RAM |
| sub0x_alt3_tls | ok | 30.0 (+3.0) | 20 (+0) | 16 (+0) | 31 (+2) | 0/0 | 2447 (+16) | 625 (+1) | 0 | - | FAIL: publish instr, no extra RAM |
| sub0x_alt4_filter | ok | 33.3 (+6.3) | 21 (+1) | 16 (+0) | 36 (+7) | 0/0 | 2463 (+32) | 632 (+8) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | reference |
| sub0x_alt1_bool | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt2_token | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt3_static | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt3_tls | ok | 11.7 (+0.0) | 20 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_alt4_filter | ok | 14.7 (+3.0) | 21 (+1) | 16 (+0) | 11 (+2) | 0/0 | 2367 (+0) | 632 (+8) | 0 | - | FAIL: publish instr, setup instr, no extra RAM |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0 | - | reference |
| sub0x_alt1_bool | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | build error: `/home/user/Sub0Pub/tests/collapse/cases/cancellation/sub0x_alt1c_expected_cpp23.cpp:15:10: error: no template named 'expected' in namespace 'std'` | | | | | | | | | | |
| sub0x_alt2_token | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt3_static | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt3_tls | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt4_filter | ok | 28.7 (+0.0) | 18 (+0) | 14 (+0) | 30 (+0) | 0/0 | 2170 (+0) | 664 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | reference |
| sub0x_alt1_bool | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | build error: `/home/user/Sub0Pub/tests/collapse/cases/cancellation/sub0x_alt1c_expected_cpp23.cpp:15:10: error: no template named 'expected' in namespace 'std'` | | | | | | | | | | |
| sub0x_alt2_token | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt3_static | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt3_tls | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_alt4_filter | ok | 11.7 (+0.0) | 17 (+0) | 14 (+0) | 8 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0 | - | reference |
| sub0x_alt1_bool | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt2_token | - | - | - | - | 32 (+0) | 0/0 | 1184 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt3_static | - | - | - | - | 37 (+5) | 0/0 | 1196 (+12) | 520 (+4) | 0 | - | FAIL: publish path, no extra RAM |
| sub0x_alt3_tls | - | - | - | - | 46 (+14) | 2/0 | 1220 (+36) | 773 (+257) | 0 | TLS | FAIL: publish path, no extra RAM, no extra dependencies |
| sub0x_alt4_filter | - | - | - | - | 43 (+11) | 0/0 | 1216 (+32) | 520 (+4) | 0 | - | FAIL: publish path, no extra RAM |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | reference |
| sub0x_alt1_bool | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt2_token | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt3_static | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt3_tls | - | - | - | - | 16 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_alt4_filter | - | - | - | - | 23 (+7) | 0/0 | 1164 (+24) | 520 (+4) | 0 | - | FAIL: publish path, no extra RAM |

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

## Case: cross_file

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 61.0 (+0.0) | 21 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2617 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 61.0 (+0.0) | 27 (+6) | 16 (+0) | 50 (+0) | 4/0 | 2649 (+32) | 656 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 130.0 (+69.0) | 56 (+35) | 128 (+112) | 73 (+23) | 2/2 | 7235 (+4618) | 1104 (+472) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 114.0 (+53.0) | 56 (+35) | 128 (+112) | 62 (+12) | 1/2 | 7055 (+4438) | 1096 (+464) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 61.0 (+0.0) | 27 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2649 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 61.0 (+0.0) | 21 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2617 (+0) | 632 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 36.0 (+0.0) | 21 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2574 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 36.0 (+0.0) | 27 (+6) | 16 (+0) | 34 (+0) | 4/0 | 2606 (+32) | 656 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 105.0 (+69.0) | 56 (+35) | 128 (+112) | 73 (+39) | 2/2 | 7187 (+4613) | 1104 (+472) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 89.0 (+53.0) | 56 (+35) | 128 (+112) | 62 (+28) | 1/2 | 7007 (+4433) | 1096 (+464) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 36.0 (+0.0) | 27 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2606 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 36.0 (+0.0) | 21 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2574 (+0) | 632 (+0) | 0 | - | PASS |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 55.0 (+0.0) | 19 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2298 (+0) | 672 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 55.0 (+0.0) | 25 (+6) | 14 (+0) | 42 (+0) | 3/0 | 2346 (+48) | 696 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 150.0 (+95.0) | 57 (+38) | 129 (+115) | 66 (+24) | 1/2 | 5022 (+2724) | 1137 (+465) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 133.0 (+78.0) | 57 (+38) | 129 (+115) | 55 (+13) | 0/2 | 4866 (+2568) | 1129 (+457) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 55.0 (+0.0) | 25 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2346 (+0) | 696 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 55.0 (+0.0) | 19 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2298 (+0) | 672 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 19 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2256 (+0) | 672 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 27.0 (+0.0) | 25 (+6) | 14 (+0) | 24 (+0) | 3/0 | 2304 (+48) | 696 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 122.0 (+95.0) | 57 (+38) | 129 (+115) | 66 (+42) | 1/2 | 4990 (+2734) | 1137 (+465) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 105.0 (+78.0) | 57 (+38) | 129 (+115) | 55 (+31) | 0/2 | 4834 (+2578) | 1129 (+457) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 27.0 (+0.0) | 25 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2304 (+0) | 696 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 27.0 (+0.0) | 19 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2256 (+0) | 672 (+0) | 0 | - | PASS |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 43 (+0) | 3/0 | 1220 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 42 (-1) | 3/0 | 1224 (+4) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 14 (-29) | 1/0 | 3024 (+1804) | 948 (+428) | 641 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 14 (-29) | 1/0 | 2704 (+1484) | 948 (+428) | 601 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 42 (+0) | 3/0 | 1224 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 43 (+0) | 3/0 | 1220 (+0) | 520 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 27 (+0) | 3/0 | 1176 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 26 (-1) | 3/0 | 1180 (+4) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 14 (-13) | 1/0 | 2984 (+1808) | 948 (+428) | 641 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 14 (-13) | 1/0 | 2664 (+1488) | 948 (+428) | 601 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 26 (+0) | 3/0 | 1180 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 27 (+0) | 3/0 | 1176 (+0) | 520 (+0) | 0 | - | PASS |

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

### gcc-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 11 (+0) | 25 (+0) | 0/0 | 2388 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 33.0 (+3.0) | 27 (+6) | 11 (+0) | 28 (+3) | 0/0 | 2425 (+37) | 664 (+32) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_virtual | ok | 130.0 (+100.0) | 56 (+35) | 128 (+117) | 73 (+48) | 2/2 | 7228 (+4840) | 1128 (+496) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 114.0 (+84.0) | 56 (+35) | 128 (+117) | 62 (+37) | 1/2 | 7048 (+4660) | 1120 (+488) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 33.0 (+0.0) | 27 (+0) | 11 (+0) | 28 (+0) | 0/0 | 2425 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 30.0 (+0.0) | 21 (+0) | 11 (+0) | 25 (+0) | 0/0 | 2388 (+0) | 632 (+0) | 0 | - | PASS |

### gcc-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 11 (+0) | 4 (+0) | 0/0 | 2321 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 10.0 (+1.0) | 27 (+6) | 11 (+0) | 5 (+1) | 0/0 | 2356 (+35) | 664 (+32) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 105.0 (+96.0) | 56 (+35) | 128 (+117) | 73 (+69) | 2/2 | 7180 (+4859) | 1128 (+496) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 89.0 (+80.0) | 56 (+35) | 128 (+117) | 62 (+58) | 1/2 | 7000 (+4679) | 1120 (+488) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 11 (+0) | 5 (+0) | 0/0 | 2356 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 21 (+0) | 11 (+0) | 4 (+0) | 0/0 | 2321 (+0) | 632 (+0) | 0 | - | PASS |

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

### clang-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | reference; PASS |
| sub0pub_virtual | ok | 146.0 (+115.0) | 54 (+36) | 129 (+119) | 65 (+38) | 1/2 | 4982 (+2868) | 1137 (+473) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 129.0 (+98.0) | 54 (+36) | 129 (+119) | 54 (+27) | 0/2 | 4826 (+2712) | 1129 (+465) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | PASS |

### clang-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2004 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 8.0 (+0.0) | 17 (+2) | 10 (+0) | 4 (+0) | 0/0 | 2024 (+20) | 664 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 121.0 (+113.0) | 53 (+38) | 129 (+119) | 65 (+61) | 1/2 | 4950 (+2946) | 1137 (+481) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 104.0 (+96.0) | 53 (+38) | 129 (+119) | 54 (+50) | 0/2 | 4794 (+2790) | 1129 (+473) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 17 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2024 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 15 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2004 (+0) | 656 (+0) | 0 | - | PASS |

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

### cm33-gcc-Os-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1172 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 28 (-2) | 0/0 | 1176 (+4) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+138) | 4/2 | 2960 (+1788) | 952 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+20) | 3/2 | 2644 (+1472) | 952 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 28 (+0) | 0/0 | 1176 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1172 (+0) | 520 (+0) | 0 | - | PASS |

### cm33-gcc-Os-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1128 (+12) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+156) | 4/2 | 2916 (+1800) | 952 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+38) | 3/2 | 2600 (+1484) | 952 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1128 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0 | - | PASS |

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

## Case: dynamic_subscriptions

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 60.8 (+0.0) | 30 (+0) | 32 (+0) | 87 (+0) | 2/2 | 3676 (+0) | 840 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 93.3 (+32.5) | 33 (+3) | 49 (+17) | 215 (+128) | 4/3 | 6872 (+3196) | 1080 (+240) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 80.0 (+19.3) | 33 (+3) | 49 (+17) | 172 (+85) | 2/3 | 6500 (+2824) | 1072 (+232) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 107.3 (+46.5) | 37 (+7) | 66 (+34) | 145 (+58) | 4/2 | 5478 (+1802) | 1008 (+168) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no extra dependencies |
| sub0x_dynamic_lean | ok | 73.8 (+13.0) | 37 (+7) | 53 (+21) | 83 (-4) | 2/2 | 4714 (+1038) | 968 (+128) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 51.0 (+0.0) | 30 (+0) | 32 (+0) | 87 (+0) | 2/2 | 3600 (+0) | 840 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 83.5 (+32.5) | 33 (+3) | 49 (+17) | 215 (+128) | 4/3 | 6796 (+3196) | 1080 (+240) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 70.3 (+19.3) | 33 (+3) | 49 (+17) | 172 (+85) | 2/3 | 6424 (+2824) | 1072 (+232) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 97.5 (+46.5) | 37 (+7) | 66 (+34) | 145 (+58) | 4/2 | 5402 (+1802) | 1008 (+168) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no extra dependencies |
| sub0x_dynamic_lean | ok | 64.0 (+13.0) | 37 (+7) | 53 (+21) | 83 (-4) | 2/2 | 4638 (+1038) | 968 (+128) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 57.0 (+0.0) | 26 (+0) | 30 (+0) | 142 (+0) | 0/2 | 3671 (+0) | 848 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 103.3 (+46.3) | 31 (+5) | 54 (+24) | 129 (-13) | 3/4 | 5113 (+1442) | 1105 (+257) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 93.8 (+36.8) | 31 (+5) | 54 (+24) | 109 (-33) | 1/4 | 4913 (+1242) | 1097 (+249) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 112.5 (+55.5) | 35 (+9) | 58 (+28) | 147 (+5) | 3/4 | 4995 (+1324) | 1008 (+160) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_lean | ok | 63.3 (+6.2) | 35 (+9) | 50 (+20) | 68 (-74) | 1/2 | 4223 (+552) | 968 (+120) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 45.8 (+0.0) | 26 (+0) | 30 (+0) | 142 (+0) | 0/2 | 3630 (+0) | 848 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 92.0 (+46.3) | 31 (+5) | 54 (+24) | 129 (-13) | 3/4 | 5072 (+1442) | 1105 (+257) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 82.5 (+36.8) | 31 (+5) | 54 (+24) | 109 (-33) | 1/4 | 4872 (+1242) | 1097 (+249) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 101.3 (+55.5) | 35 (+9) | 58 (+28) | 147 (+5) | 3/4 | 4963 (+1333) | 1008 (+160) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_lean | ok | 52.0 (+6.2) | 35 (+9) | 50 (+20) | 68 (-74) | 1/2 | 4191 (+561) | 968 (+120) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |

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

- 622 `collapse_publish`
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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 162 (+0) | 4/1 | 1636 (+0) | 548 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 46 (-116) | 4/1 | 2940 (+1304) | 924 (+376) | 357 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 46 (-116) | 4/1 | 2628 (+992) | 924 (+376) | 321 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | - | - | - | - | 35 (-127) | 3/3 | 2968 (+1332) | 912 (+364) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 35 (-127) | 5/1 | 2516 (+880) | 648 (+100) | 0 | operator delete | FAIL: no extra RAM, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 138 (+0) | 2/0 | 1520 (+0) | 548 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 46 (-92) | 4/1 | 2896 (+1376) | 924 (+376) | 357 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 46 (-92) | 4/1 | 2580 (+1060) | 924 (+376) | 321 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | - | - | - | - | 35 (-103) | 3/3 | 2924 (+1404) | 912 (+364) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_lean | - | - | - | - | 35 (-103) | 5/1 | 2472 (+952) | 648 (+100) | 0 | operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0 | - | reference |
| handwritten_erased | ok | 35.5 (+16.5) | 26 (+8) | 16 (+0) | 17 (-1) | 1/1 | 2531 (+164) | 656 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 19.0 (+0.0) | 22 (+4) | 16 (+0) | 18 (+0) | 0/0 | 2399 (+32) | 640 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 122.5 (+103.5) | 44 (+26) | 89 (+73) | 73 (+55) | 2/2 | 6744 (+4377) | 1096 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 94.5 (+75.5) | 44 (+26) | 89 (+73) | 54 (+36) | 1/2 | 6532 (+4165) | 1088 (+472) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 19.0 (+0.0) | 22 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2399 (+0) | 640 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 35.5 (+0.0) | 26 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2531 (+0) | 656 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 26 (+8) | 16 (+0) | 17 (+13) | 1/1 | 2483 (+164) | 656 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 22 (+4) | 16 (+0) | 4 (+0) | 0/0 | 2351 (+32) | 640 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_virtual | ok | 111.0 (+103.0) | 44 (+26) | 89 (+73) | 73 (+69) | 2/2 | 6668 (+4349) | 1096 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 83.0 (+75.0) | 44 (+26) | 89 (+73) | 54 (+50) | 1/2 | 6456 (+4137) | 1088 (+472) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 22 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2351 (+0) | 640 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 26 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2483 (+0) | 656 (+0) | 0 | - | PASS |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_erased | ok | 31.0 (+9.0) | 22 (+6) | 14 (+0) | 12 (-7) | 0/1 | 2218 (+96) | 688 (+32) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | reference; PASS |
| sub0pub_virtual | ok | 113.0 (+91.0) | 42 (+26) | 92 (+78) | 66 (+47) | 1/2 | 4937 (+2815) | 1121 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 94.5 (+72.5) | 42 (+26) | 92 (+78) | 55 (+36) | 0/2 | 4781 (+2659) | 1113 (+457) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 30.0 (-1.0) | 22 (+0) | 14 (+0) | 30 (+18) | 1/0 | 2233 (+15) | 688 (+0) | 0 | - | FAIL: publish path |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 22 (+6) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+32) | 688 (+32) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference; PASS |
| sub0pub_virtual | ok | 99.5 (+91.5) | 42 (+26) | 92 (+78) | 66 (+62) | 1/2 | 4905 (+2831) | 1121 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 81.0 (+73.0) | 42 (+26) | 92 (+78) | 55 (+51) | 0/2 | 4749 (+2675) | 1113 (+457) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 22 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+0) | 688 (+0) | 0 | - | PASS |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-4) | 0/1 | 1192 (+64) | 528 (+20) | 0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 19 (+0) | 0/0 | 1148 (+20) | 520 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+149) | 4/2 | 2932 (+1804) | 932 (+424) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+31) | 3/2 | 2616 (+1488) | 932 (+424) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 19 (+0) | 0/0 | 1148 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1192 (+0) | 528 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1156 (+60) | 528 (+20) | 0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 7 (+0) | 0/0 | 1116 (+20) | 520 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | - | - | - | - | 168 (+161) | 4/2 | 2884 (+1788) | 932 (+424) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 50 (+43) | 3/2 | 2568 (+1472) | 932 (+424) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 7 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1156 (+0) | 528 (+0) | 0 | - | PASS |

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

## Case: multi_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_erased | ok | 52.0 (+22.0) | 31 (+10) | 16 (+0) | 17 (-9) | 1/1 | 2627 (+196) | 672 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 38.0 (+8.0) | 27 (+6) | 16 (+0) | 35 (+9) | 0/0 | 2495 (+64) | 656 (+24) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0pub_spike | ok | 129.0 (+99.0) | 56 (+35) | 128 (+112) | 65 (+39) | 2/1 | 7228 (+4797) | 1112 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 129.0 (+99.0) | 56 (+35) | 128 (+112) | 65 (+39) | 2/1 | 7228 (+4797) | 1112 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 99.0 (+69.0) | 56 (+35) | 128 (+112) | 45 (+19) | 1/1 | 7024 (+4593) | 1104 (+472) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 52.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2627 (+0) | 672 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_erased | ok | 24.0 (+15.0) | 31 (+10) | 16 (+0) | 17 (+11) | 1/1 | 2531 (+164) | 672 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 27 (+6) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+32) | 656 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 104.0 (+95.0) | 56 (+35) | 128 (+112) | 65 (+59) | 2/1 | 7180 (+4813) | 1112 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 104.0 (+95.0) | 56 (+35) | 128 (+112) | 65 (+59) | 2/1 | 7180 (+4813) | 1112 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 74.0 (+65.0) | 56 (+35) | 128 (+112) | 45 (+39) | 1/1 | 6976 (+4609) | 1104 (+472) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 24.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2531 (+0) | 672 (+0) | 0 | - | PASS |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | reference |
| handwritten_erased | ok | 45.0 (+12.0) | 27 (+8) | 14 (+0) | 12 (-17) | 0/1 | 2314 (+144) | 704 (+32) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | reference; PASS |
| sub0pub_spike | ok | 150.0 (+117.0) | 55 (+36) | 135 (+121) | 66 (+37) | 1/2 | 5023 (+2853) | 1137 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 150.0 (+117.0) | 55 (+36) | 135 (+121) | 66 (+37) | 1/2 | 5023 (+2853) | 1137 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 133.0 (+100.0) | 55 (+36) | 135 (+121) | 55 (+26) | 0/2 | 4867 (+2697) | 1129 (+457) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 44.0 (-1.0) | 27 (+0) | 14 (+0) | 41 (+29) | 1/0 | 2314 (+0) | 704 (+0) | 0 | - | FAIL: publish path |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_erased | ok | 18.0 (+9.0) | 27 (+10) | 14 (+0) | 12 (+8) | 0/1 | 2234 (+160) | 704 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 19 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+16) | 672 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 122.0 (+113.0) | 55 (+38) | 135 (+121) | 66 (+62) | 1/2 | 4980 (+2906) | 1137 (+473) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 122.0 (+113.0) | 55 (+38) | 135 (+121) | 66 (+62) | 1/2 | 4980 (+2906) | 1137 (+473) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 105.0 (+96.0) | 55 (+38) | 135 (+121) | 55 (+51) | 0/2 | 4824 (+2750) | 1129 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 17.0 (-1.0) | 27 (+0) | 14 (+0) | 14 (+2) | 1/0 | 2239 (+5) | 704 (+0) | 0 | - | PASS |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-15) | 0/1 | 1240 (+52) | 540 (+20) | 0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 32 (+2) | 0/0 | 1200 (+12) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 162 (+132) | 4/1 | 2940 (+1752) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 162 (+132) | 4/1 | 2940 (+1752) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 44 (+14) | 3/1 | 2624 (+1436) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1240 (+0) | 540 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+3) | 0/1 | 1184 (+48) | 540 (+20) | 0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1148 (+12) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 162 (+150) | 4/1 | 2900 (+1764) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 162 (+150) | 4/1 | 2900 (+1764) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 44 (+32) | 3/1 | 2584 (+1448) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b2_static_cxx20 | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1184 (+0) | 540 (+0) | 0 | - | PASS |

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

## Case: one_receiver

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 15.0 (+0.0) | 19 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 624 (+0) | 0 | - | reference |
| handwritten_erased | ok | 31.0 (+16.0) | 25 (+6) | 16 (+0) | 17 (+6) | 1/1 | 2515 (+164) | 648 (+24) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 16.0 (+1.0) | 21 (+2) | 16 (+0) | 12 (+1) | 0/0 | 2367 (+16) | 632 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 47.0 (+32.0) | 34 (+15) | 49 (+33) | 51 (+40) | 2/0 | 5048 (+2697) | 992 (+368) | 248 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 47.0 (+32.0) | 34 (+15) | 49 (+33) | 51 (+40) | 2/0 | 5048 (+2697) | 992 (+368) | 248 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 26.0 (+11.0) | 34 (+15) | 49 (+33) | 23 (+12) | 0/0 | 4812 (+2461) | 984 (+360) | 248 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 16.0 (+0.0) | 21 (+0) | 16 (+0) | 12 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 15.0 (+0.0) | 19 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 31.0 (+0.0) | 25 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2515 (+0) | 648 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 19 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 624 (+0) | 0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 25 (+6) | 16 (+0) | 17 (+13) | 1/1 | 2483 (+164) | 648 (+24) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 21 (+2) | 16 (+0) | 4 (+0) | 0/0 | 2335 (+16) | 632 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 21 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2335 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 19 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 25 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2483 (+0) | 648 (+0) | 0 | - | PASS |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_erased | ok | 26.0 (+8.0) | 21 (+4) | 14 (+0) | 12 (-2) | 0/1 | 2202 (+96) | 680 (+16) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 17.0 (-1.0) | 19 (+2) | 14 (+0) | 13 (-1) | 0/0 | 2122 (+16) | 672 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 78.0 (+60.0) | 32 (+15) | 54 (+40) | 66 (+52) | 1/2 | 4442 (+2336) | 1033 (+369) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 78.0 (+60.0) | 32 (+15) | 54 (+40) | 66 (+52) | 1/2 | 4442 (+2336) | 1033 (+369) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 71.0 (+53.0) | 32 (+15) | 54 (+40) | 55 (+41) | 0/2 | 4286 (+2180) | 1025 (+361) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 17.0 (+0.0) | 19 (+0) | 14 (+0) | 13 (+0) | 0/0 | 2122 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 25.0 (-1.0) | 21 (+0) | 14 (+0) | 22 (+10) | 1/0 | 2214 (+12) | 680 (+0) | 0 | - | FAIL: publish path |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 21 (+5) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+32) | 680 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| handwritten_runtime | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference; PASS |
| sub0pub_spike | ok | 69.0 (+61.0) | 32 (+16) | 54 (+40) | 66 (+62) | 1/2 | 4426 (+2352) | 1033 (+377) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 69.0 (+61.0) | 32 (+16) | 54 (+40) | 66 (+62) | 1/2 | 4426 (+2352) | 1033 (+377) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 62.0 (+54.0) | 32 (+16) | 54 (+40) | 55 (+51) | 0/2 | 4270 (+2196) | 1025 (+369) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 21 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+0) | 680 (+0) | 0 | - | PASS |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 15 (+0) | 0/0 | 1128 (+0) | 512 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+0) | 0/1 | 1176 (+48) | 524 (+12) | 0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 17 (+2) | 0/0 | 1140 (+12) | 516 (+4) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 146 (+131) | 2/0 | 2668 (+1540) | 660 (+148) | 64 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 146 (+131) | 2/0 | 2668 (+1540) | 660 (+148) | 64 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 31 (+16) | 1/0 | 2364 (+1236) | 660 (+148) | 64 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 17 (+0) | 0/0 | 1140 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 15 (+0) | 0/0 | 1128 (+0) | 512 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1176 (+0) | 524 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1104 (+0) | 512 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1152 (+48) | 524 (+12) | 0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 7 (+0) | 0/0 | 1112 (+8) | 516 (+4) | 0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 7 (+0) | 0/0 | 1112 (+0) | 516 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1104 (+0) | 512 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1152 (+0) | 524 (+0) | 0 | - | PASS |

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

## Case: publisher_ergonomics

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_erased | ok | 52.0 (+22.0) | 31 (+10) | 16 (+0) | 17 (-9) | 1/1 | 2627 (+196) | 672 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 38.0 (+8.0) | 27 (+6) | 16 (+0) | 35 (+9) | 0/0 | 2495 (+64) | 656 (+24) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 35 (+0) | 0/0 | 2495 (+0) | 656 (+0) | 0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 52.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2627 (+0) | 672 (+0) | 0 | - | PASS |
| alt6_static_bound | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | PASS |
| alt7_deducing_this_mixin | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt7_deducing_this_mixin.cpp:34:18: error: expected identifier before ‘this’` | | | | | | | | | | |
| alt8_deducing_this_callsite | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt8_deducing_this_callsite.cpp:33:15: error: expected identifier before ‘this’` | | | | | | | | | | |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_erased | ok | 24.0 (+15.0) | 31 (+10) | 16 (+0) | 17 (+11) | 1/1 | 2531 (+164) | 672 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 10.0 (+1.0) | 27 (+6) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+32) | 656 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | 10.0 (+0.0) | 27 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2399 (+0) | 656 (+0) | 0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 24.0 (+0.0) | 31 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2531 (+0) | 672 (+0) | 0 | - | PASS |
| alt6_static_bound | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| alt7_deducing_this_mixin | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt7_deducing_this_mixin.cpp:34:18: error: expected identifier before ‘this’` | | | | | | | | | | |
| alt8_deducing_this_callsite | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt8_deducing_this_callsite.cpp:33:15: error: expected identifier before ‘this’` | | | | | | | | | | |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | reference |
| handwritten_erased | ok | 45.0 (+12.0) | 27 (+8) | 14 (+0) | 12 (-17) | 0/1 | 2314 (+144) | 704 (+32) | 0 | - | reference; FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | reference; PASS |
| alt1_baseline_template (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 37.0 (+4.0) | 25 (+6) | 14 (+0) | 33 (+4) | 0/0 | 2218 (+48) | 696 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| alt4_call_site_out (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 44.0 (-1.0) | 27 (+0) | 14 (+0) | 41 (+29) | 1/0 | 2314 (+0) | 704 (+0) | 0 | - | FAIL: publish path |
| alt6_static_bound | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_erased | ok | 18.0 (+9.0) | 27 (+10) | 14 (+0) | 12 (+8) | 0/1 | 2234 (+160) | 704 (+40) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | 9.0 (+0.0) | 19 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+16) | 672 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | 10.0 (+1.0) | 25 (+6) | 14 (+0) | 7 (+3) | 0/0 | 2154 (+64) | 696 (+24) | 0 | - | FAIL: setup instr, publish path, no extra RAM |
| alt4_call_site_out (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | 17.0 (-1.0) | 27 (+0) | 14 (+0) | 14 (+2) | 1/0 | 2239 (+5) | 704 (+0) | 0 | - | PASS |
| alt6_static_bound | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | 9.0 (+0.0) | 19 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+0) | 672 (+0) | 0 | - | PASS |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (-15) | 0/1 | 1240 (+52) | 540 (+20) | 0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 32 (+2) | 0/0 | 1200 (+12) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1204 (+4) | 532 (+0) | 0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | - | - | - | - | 32 (+0) | 0/0 | 1200 (+0) | 532 (+0) | 0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1240 (+0) | 540 (+0) | 0 | - | PASS |
| alt6_static_bound | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | PASS |
| alt7_deducing_this_mixin | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt7_deducing_this_mixin.cpp:34:18: error: expected identifier before 'this'` | | | | | | | | | | |
| alt8_deducing_this_callsite | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt8_deducing_this_callsite.cpp:33:15: error: expected identifier before 'this'` | | | | | | | | | | |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+3) | 0/1 | 1184 (+48) | 540 (+20) | 0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | - | - | - | - | 12 (+0) | 0/0 | 1148 (+12) | 532 (+12) | 0 | - | reference; FAIL: no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1152 (+4) | 532 (+0) | 0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | - | - | - | - | 12 (+0) | 0/0 | 1148 (+0) | 532 (+0) | 0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1184 (+0) | 540 (+0) | 0 | - | PASS |
| alt6_static_bound | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | PASS |
| alt7_deducing_this_mixin | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt7_deducing_this_mixin.cpp:34:18: error: expected identifier before 'this'` | | | | | | | | | | |
| alt8_deducing_this_callsite | build error: `/home/user/Sub0Pub/tests/collapse/cases/publisher_ergonomics/alt8_deducing_this_callsite.cpp:33:15: error: expected identifier before 'this'` | | | | | | | | | | |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 37.0 (+0.0) | 31 (+0) | 32 (+0) | 33 (+0) | 0/0 | 3208 (+0) | 800 (+0) | 0 | - | reference |
| sub0x_bridge_broker | ok | 37.0 (+0.0) | 51 (+20) | 86 (+54) | 33 (+0) | 0/0 | 4669 (+1461) | 944 (+144) | 0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_inverted | ok | 74.0 (+37.0) | 75 (+44) | 137 (+105) | 36 (+3) | 1/1 | 5382 (+2174) | 1040 (+240) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_bridge_slots | ok | 37.0 (+0.0) | 31 (+0) | 32 (+0) | 33 (+0) | 0/0 | 3256 (+48) | 800 (+0) | 0 | - | PASS |
| sub0x_bridge_slots_cpp23 | ok | 37.0 (+0.0) | 31 (+0) | 32 (+0) | 33 (+0) | 0/0 | 3256 (+48) | 800 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 31 (+0) | 32 (+0) | 6 (+0) | 0/0 | 3112 (+0) | 800 (+0) | 0 | - | reference |
| sub0x_bridge_broker | ok | 9.0 (+0.0) | 51 (+20) | 86 (+54) | 6 (+0) | 0/0 | 4573 (+1461) | 944 (+144) | 0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_inverted | ok | 51.0 (+42.0) | 75 (+44) | 137 (+105) | 36 (+30) | 1/1 | 5318 (+2206) | 1040 (+240) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_bridge_slots | ok | 9.0 (+0.0) | 31 (+0) | 32 (+0) | 6 (+0) | 0/0 | 3160 (+48) | 800 (+0) | 0 | - | PASS |
| sub0x_bridge_slots_cpp23 | ok | 9.0 (+0.0) | 31 (+0) | 32 (+0) | 6 (+0) | 0/0 | 3160 (+48) | 800 (+0) | 0 | - | PASS |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 58.0 (+0.0) | 27 (+0) | 30 (+0) | 44 (+0) | 0/1 | 3078 (+0) | 808 (+0) | 0 | - | reference |
| sub0x_bridge_broker | ok | 57.0 (-1.0) | 40 (+13) | 67 (+37) | 44 (+0) | 0/1 | 4150 (+1072) | 944 (+136) | 0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_inverted | ok | 68.0 (+10.0) | 57 (+30) | 113 (+83) | 28 (-16) | 0/1 | 4642 (+1564) | 1032 (+224) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_slots | ok | 58.0 (+0.0) | 27 (+0) | 30 (+0) | 44 (+0) | 0/1 | 3108 (+30) | 808 (+0) | 0 | - | PASS |
| sub0x_bridge_slots_cpp23 | ok | 58.0 (+0.0) | 27 (+0) | 30 (+0) | 44 (+0) | 0/1 | 3108 (+30) | 808 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 34.0 (+0.0) | 27 (+0) | 30 (+0) | 29 (+0) | 0/1 | 3021 (+0) | 808 (+0) | 0 | - | reference |
| sub0x_bridge_broker | ok | 33.0 (-1.0) | 40 (+13) | 67 (+37) | 29 (+0) | 0/1 | 4102 (+1081) | 944 (+136) | 0 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_inverted | ok | 42.0 (+8.0) | 57 (+30) | 113 (+83) | 28 (-1) | 0/1 | 4578 (+1557) | 1032 (+224) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_slots | ok | 34.0 (+0.0) | 27 (+0) | 30 (+0) | 29 (+0) | 0/1 | 3051 (+30) | 808 (+0) | 0 | - | PASS |
| sub0x_bridge_slots_cpp23 | ok | 34.0 (+0.0) | 27 (+0) | 30 (+0) | 29 (+0) | 0/1 | 3051 (+30) | 808 (+0) | 0 | - | PASS |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 45 (+0) | 1/0 | 1568 (+0) | 552 (+0) | 0 | - | reference |
| sub0x_bridge_broker | - | - | - | - | 48 (+3) | 1/0 | 3576 (+2008) | 668 (+116) | 0 | operator delete | FAIL: publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_inverted | - | - | - | - | 24 (-21) | 0/1 | 3720 (+2152) | 680 (+128) | 0 | operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_bridge_slots | - | - | - | - | 45 (+0) | 1/0 | 1568 (+0) | 552 (+0) | 0 | - | PASS |
| sub0x_bridge_slots_cpp23 | - | - | - | - | 45 (+0) | 1/0 | 1568 (+0) | 552 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1488 (+0) | 552 (+0) | 0 | - | reference |
| sub0x_bridge_broker | - | - | - | - | 12 (+0) | 0/0 | 3488 (+2000) | 668 (+116) | 0 | operator delete | FAIL: no extra RAM, no extra dependencies |
| sub0x_bridge_inverted | - | - | - | - | 24 (+12) | 0/1 | 3664 (+2176) | 680 (+128) | 0 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_bridge_slots | - | - | - | - | 12 (+0) | 0/0 | 1488 (+0) | 552 (+0) | 0 | - | PASS |
| sub0x_bridge_slots_cpp23 | - | - | - | - | 12 (+0) | 0/0 | 1488 (+0) | 552 (+0) | 0 | - | PASS |

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

## Case: static_dynamic_bridge_empty

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 23.0 (+0.0) | 19 (+0) | 16 (+0) | 19 (+0) | 0/0 | 2367 (+0) | 624 (+0) | 0 | - | reference |
| handwritten_registry | ok | 26.0 (+3.0) | 25 (+6) | 16 (+0) | 22 (+3) | 0/0 | 2576 (+209) | 720 (+96) | 0 | pure virtual | reference; FAIL: publish instr, setup instr, publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 27.0 (+1.0) | 28 (+3) | 35 (+19) | 24 (+2) | 0/0 | 3041 (+465) | 744 (+24) | 0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 33.0 (+7.0) | 52 (+27) | 86 (+70) | 29 (+7) | 0/0 | 4778 (+2202) | 944 (+224) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 26.0 (+0.0) | 25 (+0) | 16 (+0) | 22 (+0) | 0/0 | 2576 (+0) | 720 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 19 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2335 (+0) | 624 (+0) | 0 | - | reference |
| handwritten_registry | ok | 12.0 (+3.0) | 25 (+6) | 16 (+0) | 9 (+3) | 0/0 | 2544 (+209) | 720 (+96) | 0 | pure virtual | reference; FAIL: publish instr, setup instr, publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 13.0 (+1.0) | 28 (+3) | 35 (+19) | 10 (+1) | 0/0 | 2993 (+449) | 744 (+24) | 0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 20.0 (+8.0) | 52 (+27) | 86 (+70) | 23 (+14) | 0/0 | 4714 (+2170) | 944 (+224) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 12.0 (+0.0) | 25 (+0) | 16 (+0) | 9 (+0) | 0/0 | 2544 (+0) | 720 (+0) | 0 | - | PASS |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 24.0 (+0.0) | 17 (+0) | 14 (+0) | 20 (+0) | 0/0 | 2122 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_registry | ok | 35.0 (+11.0) | 23 (+6) | 14 (+0) | 44 (+24) | 0/1 | 2278 (+156) | 736 (+72) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 35.0 (+0.0) | 26 (+3) | 26 (+12) | 44 (+0) | 0/1 | 2592 (+314) | 760 (+24) | 0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 50.0 (+15.0) | 40 (+17) | 67 (+53) | 28 (-16) | 0/1 | 4227 (+1949) | 944 (+208) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 35.0 (+0.0) | 23 (+0) | 14 (+0) | 44 (+0) | 0/1 | 2278 (+0) | 736 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_registry | ok | 20.0 (+11.0) | 23 (+6) | 14 (+0) | 29 (+25) | 0/1 | 2246 (+172) | 736 (+72) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | 20.0 (+0.0) | 26 (+3) | 26 (+12) | 29 (+0) | 0/1 | 2560 (+314) | 760 (+24) | 0 | - | FAIL: setup instr, teardown instr, no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | 33.0 (+13.0) | 40 (+17) | 67 (+53) | 28 (-1) | 0/1 | 4179 (+1933) | 944 (+208) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra RAM, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | 20.0 (+0.0) | 23 (+0) | 14 (+0) | 29 (+0) | 0/1 | 2246 (+0) | 736 (+0) | 0 | - | PASS |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 22 (+0) | 0/0 | 1144 (+0) | 512 (+0) | 0 | - | reference |
| handwritten_registry | - | - | - | - | 28 (+6) | 0/0 | 1180 (+36) | 548 (+36) | 0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | - | - | - | - | 28 (+0) | 0/0 | 2716 (+1536) | 656 (+108) | 0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | - | - | - | - | 21 (-7) | 1/0 | 3564 (+2384) | 668 (+120) | 0 | operator delete | FAIL: no extra RAM, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | - | - | - | - | 28 (+0) | 0/0 | 1180 (+0) | 548 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 512 (+0) | 0 | - | reference |
| handwritten_registry | - | - | - | - | 18 (+6) | 0/0 | 1152 (+36) | 548 (+36) | 0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | - | - | - | - | 18 (+0) | 0/0 | 2688 (+1536) | 656 (+108) | 0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | - | - | - | - | 21 (+3) | 0/0 | 3520 (+2368) | 668 (+120) | 0 | operator delete | FAIL: publish path, no extra RAM, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | - | - | - | - | 18 (+0) | 0/0 | 1152 (+0) | 548 (+0) | 0 | - | PASS |

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 27.0 (+0.0) | 22 (+4) | 16 (+0) | 23 (+0) | 0/0 | 2415 (+32) | 640 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 27.0 (+0.0) | 22 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2415 (+0) | 640 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 205.0 (+178.0) | 62 (+44) | 124 (+108) | 115 (+92) | 3/2 | 5973 (+3590) | 1056 (+440) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 143.0 (+116.0) | 62 (+44) | 124 (+108) | 76 (+53) | 1/2 | 5521 (+3138) | 1016 (+400) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 14.0 (+0.0) | 22 (+4) | 16 (+0) | 11 (+0) | 0/0 | 2383 (+32) | 640 (+24) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 14.0 (+0.0) | 22 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2383 (+0) | 640 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 189.0 (+175.0) | 62 (+44) | 124 (+108) | 115 (+104) | 3/2 | 5957 (+3606) | 1056 (+440) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 127.0 (+113.0) | 62 (+44) | 124 (+108) | 76 (+65) | 1/2 | 5505 (+3154) | 1016 (+400) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 33 `collapse_setup`
- 16 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0 | - | reference; PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 238.0 (+209.0) | 60 (+44) | 121 (+107) | 136 (+111) | 2/4 | 5425 (+3287) | 1048 (+392) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 141.0 (+112.0) | 60 (+44) | 121 (+107) | 74 (+49) | 0/2 | 4917 (+2779) | 1016 (+360) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0 | - | reference; PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 220.0 (+206.0) | 60 (+44) | 121 (+107) | 136 (+126) | 2/4 | 5409 (+3319) | 1048 (+392) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_route_lean | ok | 123.0 (+109.0) | 60 (+44) | 121 (+107) | 74 (+64) | 0/2 | 4901 (+2811) | 1016 (+360) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_route (bytes)</summary>

- 553 `collapse_publish`
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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 25 (+0) | 0/0 | 1168 (+20) | 520 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 25 (+0) | 0/0 | 1168 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | - | - | - | - | 20 (-5) | 0/2 | 3056 (+1908) | 928 (+420) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_route_lean | - | - | - | - | 20 (-5) | 0/2 | 2696 (+1548) | 672 (+164) | 0 | operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 13 (+0) | 0/0 | 1132 (+20) | 520 (+12) | 0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 13 (+0) | 0/0 | 1132 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | - | - | - | - | 20 (+7) | 0/2 | 3036 (+1924) | 928 (+420) | 0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_route_lean | - | - | - | - | 20 (+7) | 0/2 | 2676 (+1564) | 672 (+164) | 0 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 24 `collapse_setup`
- 8 `(anonymous namespace)::node`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

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

## Case: two_domains

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 31.0 (+0.0) | 21 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 38.0 (+7.0) | 27 (+6) | 16 (+0) | 34 (+7) | 0/0 | 2495 (+64) | 664 (+32) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 38.0 (+0.0) | 27 (+0) | 16 (+0) | 34 (+0) | 0/0 | 2495 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 31.0 (+0.0) | 21 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 178.0 (+147.0) | 107 (+86) | 233 (+217) | 128 (+101) | 3/2 | 6286 (+3855) | 1192 (+560) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 102.0 (+71.0) | 107 (+86) | 222 (+206) | 59 (+32) | 1/2 | 5670 (+3239) | 1160 (+528) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 10.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 11.0 (+1.0) | 27 (+6) | 16 (+0) | 7 (+1) | 0/0 | 2399 (+32) | 664 (+32) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 11.0 (+0.0) | 27 (+0) | 16 (+0) | 7 (+0) | 0/0 | 2399 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 10.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 153.0 (+143.0) | 107 (+86) | 233 (+217) | 128 (+122) | 3/2 | 6238 (+3871) | 1192 (+560) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 77.0 (+67.0) | 107 (+86) | 222 (+206) | 59 (+53) | 1/2 | 5622 (+3255) | 1160 (+528) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 119 `collapse_publish`
- 77 `collapse_setup`
- 16 `(anonymous namespace)::sensorA`
- 8 `(anonymous namespace)::sensorB`

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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 34.0 (+0.0) | 21 (+2) | 14 (+0) | 31 (+0) | 0/0 | 2202 (+16) | 680 (+8) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 34.0 (+0.0) | 21 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2202 (+0) | 680 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 203.0 (+169.0) | 79 (+60) | 195 (+181) | 138 (+107) | 2/4 | 5940 (+3754) | 1176 (+504) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 97.0 (+63.0) | 79 (+60) | 164 (+150) | 53 (+22) | 0/2 | 4956 (+2770) | 1136 (+464) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | reference |
| handwritten_runtime | ok | 9.0 (+0.0) | 18 (+1) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+16) | 664 (+0) | 0 | - | reference; FAIL: setup instr |
| sub0x_b1_wire (vs handwritten_runtime) | ok | 9.0 (+0.0) | 18 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 175.0 (+166.0) | 79 (+62) | 195 (+181) | 138 (+132) | 2/4 | 5908 (+3818) | 1176 (+512) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | 69.0 (+60.0) | 79 (+62) | 164 (+150) | 53 (+47) | 0/2 | 4924 (+2834) | 1136 (+472) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>clang-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 42 `collapse_setup`
- 8 `(anonymous namespace)::sensorB`
- 4 `(anonymous namespace)::controllerB`

</details>

<details><summary>clang-O2: largest symbols added by sub0x_dynamic_domain (bytes)</summary>

- 581 `collapse_publish`
- 490 `sub0x::Subscribe<(anonymous namespace)::Sample>::~Subscribe()`
- 440 `collapse::Slot<sub0x::Domain<(anonymous namespace)::Sample> >::reset()`
- 409 `collapse_setup`
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

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 37 (+7) | 0/0 | 1220 (+32) | 532 (+12) | 0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 37 (+0) | 0/0 | 1220 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | - | - | - | - | 23 (-7) | 0/2 | 3760 (+2572) | 1008 (+488) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_domain_lean | - | - | - | - | 45 (+15) | 0/2 | 3848 (+2660) | 748 (+228) | 0 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | reference |
| handwritten_runtime | - | - | - | - | 16 (+4) | 0/0 | 1164 (+28) | 532 (+12) | 0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | - | - | - | - | 16 (+0) | 0/0 | 1164 (+0) | 532 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | - | - | - | - | 23 (+11) | 0/2 | 3720 (+2584) | 1008 (+488) | 0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |
| sub0x_dynamic_domain_lean | - | - | - | - | 45 (+33) | 0/2 | 3808 (+2672) | 748 (+228) | 0 | operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by handwritten_runtime (bytes)</summary>

- 92 `collapse_publish`
- 52 `collapse_setup`
- 8 `(anonymous namespace)::sensorA`
- 4 `(anonymous namespace)::sensorB`

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
- 112 `collapse_publish`
- 100 `__sigtramp`

</details>

## Case: zero_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 22 (+4) | 16 (+0) | 17 (+13) | 1/1 | 2467 (+148) | 640 (+24) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 21 (+3) | 16 (+0) | 4 (+0) | 0/0 | 3022 (+703) | 728 (+112) | 58 | operator delete | FAIL: setup instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 22 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2467 (+0) | 640 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | reference |
| handwritten_erased | ok | 22.0 (+14.0) | 22 (+4) | 16 (+0) | 17 (+13) | 1/1 | 2467 (+148) | 640 (+24) | 0 | - | reference; FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 21 (+3) | 16 (+0) | 4 (+0) | 0/0 | 3022 (+703) | 728 (+112) | 58 | operator delete | FAIL: setup instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 22.0 (+0.0) | 22 (+0) | 16 (+0) | 17 (+0) | 1/1 | 2467 (+0) | 640 (+0) | 0 | - | PASS |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 18 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+16) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 18 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| handwritten_erased | ok | 8.0 (+0.0) | 18 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+16) | 0 | - | reference; FAIL: setup instr, no extra RAM |
| sub0pub_spike | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | 8.0 (+0.0) | 18 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+0) | 0 | - | PASS |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1140 (+44) | 520 (+12) | 0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 1756 (+660) | 612 (+104) | 0 | operator delete | FAIL: no extra RAM, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1140 (+0) | 520 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | reference |
| handwritten_erased | - | - | - | - | 15 (+8) | 0/1 | 1140 (+44) | 520 (+12) | 0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | - | - | - | - | 7 (+0) | 0/0 | 1756 (+660) | 612 (+104) | 0 | operator delete | FAIL: no extra RAM, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | - | - | - | - | 15 (+0) | 0/1 | 1140 (+0) | 520 (+0) | 0 | - | PASS |

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


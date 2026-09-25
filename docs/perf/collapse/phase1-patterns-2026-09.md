# Collapse evidence (issue #9)

Final-link evidence per case, build and form; every variant is compared with `handwritten` (equal-work reference, same build and form). Deltas in parentheses. instr = callgrind instructions (publish: per publication of 1000). path = static instructions of `collapse_publish` plus directly reachable functions.

- **gcc-O2**: `g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` `-O2`
- **clang-O2**: `Ubuntu clang version 18.1.3 (1ubuntu1)` `-O2`
- **cm33-gcc-Os**: `arm-none-eabi-g++ (15:13.2.rel1-2) 13.2.1 20231009` `-Os -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -fno-rtti -DCOLLAPSE_NO_STDIO`
- **gcc-O2-lto**: `g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` `-O2 -flto`
- **clang-O2-lto**: `Ubuntu clang version 18.1.3 (1ubuntu1)` `-O2 -flto`
- **cm33-gcc-Os-lto**: `arm-none-eabi-g++ (15:13.2.rel1-2) 13.2.1 20231009` `-Os -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -fno-rtti -DCOLLAPSE_NO_STDIO -flto`

## Case: cross_file

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 61.0 (+0.0) | 21 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2617 (+0) | 632 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 130.0 (+69.0) | 56 (+35) | 128 (+112) | 73 (+23) | 2/2 | 7235 (+4618) | 1104 (+472) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 68.0 (+7.0) | 29 (+8) | 16 (+0) | 56 (+6) | 4/0 | 2681 (+64) | 672 (+40) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 61.0 (+0.0) | 21 (+0) | 16 (+0) | 50 (+0) | 4/0 | 2617 (+0) | 632 (+0) | 0 | - | PASS |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 36.0 (+0.0) | 21 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2574 (+0) | 632 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 105.0 (+69.0) | 56 (+35) | 128 (+112) | 73 (+39) | 2/2 | 7187 (+4613) | 1104 (+472) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 43.0 (+7.0) | 29 (+8) | 16 (+0) | 40 (+6) | 4/0 | 2638 (+64) | 672 (+40) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 36.0 (+0.0) | 21 (+0) | 16 (+0) | 34 (+0) | 4/0 | 2574 (+0) | 632 (+0) | 0 | - | PASS |

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

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 122 `collapse_publish`
- 91 `collapse_setup`
- 24 `(anonymous namespace)::bus`
- 8 `(anonymous namespace)::sensor`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 55.0 (+0.0) | 19 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2298 (+0) | 672 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 150.0 (+95.0) | 57 (+38) | 129 (+115) | 66 (+24) | 1/2 | 5022 (+2724) | 1137 (+465) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 61.0 (+6.0) | 25 (+6) | 14 (+0) | 48 (+6) | 3/0 | 2378 (+80) | 696 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 55.0 (+0.0) | 19 (+0) | 14 (+0) | 42 (+0) | 3/0 | 2298 (+0) | 672 (+0) | 0 | - | PASS |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 19 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2256 (+0) | 672 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 122.0 (+95.0) | 57 (+38) | 129 (+115) | 66 (+42) | 1/2 | 4990 (+2734) | 1137 (+465) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 33.0 (+6.0) | 25 (+6) | 14 (+0) | 30 (+6) | 3/0 | 2336 (+80) | 696 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 27.0 (+0.0) | 19 (+0) | 14 (+0) | 24 (+0) | 3/0 | 2256 (+0) | 672 (+0) | 0 | - | PASS |

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

<details><summary>clang-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 88 `collapse_publish`
- 73 `collapse_setup`
- 8 `_ZN12_GLOBAL__N_13busE.2`
- 8 `_ZN12_GLOBAL__N_13busE.1`
- 8 `_ZN12_GLOBAL__N_13busE.0`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 43 (+0) | 3/0 | 1220 (+0) | 520 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 14 (-29) | 1/0 | 3024 (+1804) | 948 (+428) | 641 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 43 (+0) | 3/0 | 1236 (+16) | 536 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 43 (+0) | 3/0 | 1220 (+0) | 520 (+0) | 0 | - | PASS |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 27 (+0) | 3/0 | 1176 (+0) | 520 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 14 (-13) | 1/0 | 2984 (+1808) | 948 (+428) | 641 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 27 (+0) | 3/0 | 1192 (+16) | 536 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 27 (+0) | 3/0 | 1176 (+0) | 520 (+0) | 0 | - | PASS |

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

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 52 `collapse_setup`
- 12 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::sensor`

</details>

### gcc-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 11 (+0) | 25 (+0) | 0/0 | 2388 (+0) | 632 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 130.0 (+100.0) | 56 (+35) | 128 (+117) | 73 (+48) | 2/2 | 7228 (+4840) | 1128 (+496) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 34.0 (+4.0) | 29 (+8) | 11 (+0) | 29 (+4) | 0/0 | 2441 (+53) | 664 (+32) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 30.0 (+0.0) | 21 (+0) | 11 (+0) | 25 (+0) | 0/0 | 2388 (+0) | 632 (+0) | 0 | - | PASS |

### gcc-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 11 (+0) | 4 (+0) | 0/0 | 2321 (+0) | 632 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 105.0 (+96.0) | 56 (+35) | 128 (+117) | 73 (+69) | 2/2 | 7180 (+4859) | 1128 (+496) | 235 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 11.0 (+2.0) | 29 (+8) | 11 (+0) | 6 (+2) | 0/0 | 2375 (+54) | 664 (+32) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 9.0 (+0.0) | 21 (+0) | 11 (+0) | 4 (+0) | 0/0 | 2321 (+0) | 632 (+0) | 0 | - | PASS |

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

<details><summary>gcc-O2-lto: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 91 `collapse_setup`
- 91 `collapse_publish`
- 24 `(anonymous namespace)::bus`
- 8 `(anonymous namespace)::sensor`

</details>

### clang-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 146.0 (+115.0) | 54 (+36) | 129 (+119) | 65 (+38) | 1/2 | 4982 (+2868) | 1137 (+473) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 31.0 (+0.0) | 18 (+0) | 10 (+0) | 27 (+0) | 0/0 | 2114 (+0) | 664 (+0) | 0 | - | PASS |

### clang-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 10 (+0) | 4 (+0) | 0/0 | 2004 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 121.0 (+113.0) | 53 (+38) | 129 (+119) | 65 (+61) | 1/2 | 4950 (+2946) | 1137 (+481) | 600 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 17 (+2) | 10 (+0) | 4 (+0) | 0/0 | 2024 (+20) | 664 (+8) | 0 | - | FAIL: setup instr, no extra RAM |
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

### cm33-gcc-Os-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1172 (+0) | 520 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 168 (+138) | 4/2 | 2960 (+1788) | 952 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 30 (+0) | 0/0 | 1188 (+16) | 536 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1172 (+0) | 520 (+0) | 0 | - | PASS |

### cm33-gcc-Os-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 168 (+156) | 4/2 | 2916 (+1800) | 952 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 14 (+2) | 0/0 | 1140 (+24) | 536 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1116 (+0) | 520 (+0) | 0 | - | PASS |

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

<details><summary>cm33-gcc-Os-lto: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 52 `collapse_setup`
- 12 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::sensor`

</details>

## Case: dynamic_subscriptions

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 60.8 (+0.0) | 30 (+0) | 32 (+0) | 87 (+0) | 2/2 | 3676 (+0) | 840 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 93.3 (+32.5) | 33 (+3) | 49 (+17) | 215 (+128) | 4/3 | 6872 (+3196) | 1080 (+240) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 107.3 (+46.5) | 37 (+7) | 66 (+34) | 145 (+58) | 4/2 | 5478 (+1802) | 1008 (+168) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 51.0 (+0.0) | 30 (+0) | 32 (+0) | 87 (+0) | 2/2 | 3600 (+0) | 840 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 83.5 (+32.5) | 33 (+3) | 49 (+17) | 215 (+128) | 4/3 | 6796 (+3196) | 1080 (+240) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 97.5 (+46.5) | 37 (+7) | 66 (+34) | 145 (+58) | 4/2 | 5402 (+1802) | 1008 (+168) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no extra dependencies |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 57.0 (+0.0) | 26 (+0) | 30 (+0) | 142 (+0) | 0/2 | 3671 (+0) | 848 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 103.3 (+46.3) | 31 (+5) | 54 (+24) | 129 (-13) | 3/4 | 5113 (+1442) | 1105 (+257) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 112.5 (+55.5) | 35 (+9) | 58 (+28) | 147 (+5) | 3/4 | 4995 (+1324) | 1008 (+160) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 45.8 (+0.0) | 26 (+0) | 30 (+0) | 142 (+0) | 0/2 | 3630 (+0) | 848 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 92.0 (+46.3) | 31 (+5) | 54 (+24) | 129 (-13) | 3/4 | 5072 (+1442) | 1105 (+257) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | 101.3 (+55.5) | 35 (+9) | 58 (+28) | 147 (+5) | 3/4 | 4963 (+1333) | 1008 (+160) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 162 (+0) | 4/1 | 1636 (+0) | 548 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 46 (-116) | 4/1 | 2940 (+1304) | 924 (+376) | 357 | TLS, operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | - | - | - | - | 35 (-127) | 3/3 | 2968 (+1332) | 912 (+364) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 138 (+0) | 2/0 | 1520 (+0) | 548 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 46 (-92) | 4/1 | 2896 (+1376) | 924 (+376) | 357 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | - | - | - | - | 35 (-103) | 3/3 | 2924 (+1404) | 912 (+364) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |

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

## Case: filters

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 122.5 (+103.5) | 44 (+26) | 89 (+73) | 73 (+55) | 2/2 | 6744 (+4377) | 1096 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 19.0 (+0.0) | 24 (+6) | 16 (+0) | 18 (+0) | 0/0 | 2399 (+32) | 656 (+40) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 19.0 (+0.0) | 18 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2367 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 35.5 (+16.5) | 26 (+8) | 16 (+0) | 17 (-1) | 1/1 | 2531 (+164) | 656 (+40) | 0 | - | FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 111.0 (+103.0) | 44 (+26) | 89 (+73) | 73 (+69) | 2/2 | 6668 (+4349) | 1096 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 24 (+6) | 16 (+0) | 4 (+0) | 0/0 | 2351 (+32) | 656 (+40) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 22.0 (+14.0) | 26 (+8) | 16 (+0) | 17 (+13) | 1/1 | 2483 (+164) | 656 (+40) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |

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

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 47 `collapse_setup`
- 16 `(anonymous namespace)::bus`
- 8 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 79 `collapse_publish`
- 61 `collapse_setup`
- 55 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::sensor`
- 16 `(anonymous namespace)::bus`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 113.0 (+91.0) | 42 (+26) | 92 (+78) | 66 (+47) | 1/2 | 4937 (+2815) | 1121 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 22.0 (+0.0) | 16 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2122 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 30.0 (+8.0) | 22 (+6) | 14 (+0) | 30 (+11) | 1/0 | 2233 (+111) | 688 (+32) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 99.5 (+91.5) | 42 (+26) | 92 (+78) | 66 (+62) | 1/2 | 4905 (+2831) | 1121 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 8.0 (+0.0) | 22 (+6) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+32) | 688 (+32) | 0 | - | FAIL: setup instr, no extra RAM |

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

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 48 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerENS1_11EvenMonitorEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 43 `collapse_setup`
- 16 `(anonymous namespace)::bus`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 168 (+149) | 4/2 | 2932 (+1804) | 932 (+424) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 19 (+0) | 0/0 | 1156 (+28) | 524 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 19 (+0) | 0/0 | 1128 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (-4) | 0/1 | 1192 (+64) | 528 (+20) | 0 | - | FAIL: no extra indirect calls, no extra RAM |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 168 (+161) | 4/2 | 2884 (+1788) | 932 (+424) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1124 (+28) | 524 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (+8) | 0/1 | 1156 (+60) | 528 (+20) | 0 | - | FAIL: publish path, no extra indirect calls, no extra RAM |

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

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 32 `collapse_setup`
- 8 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 40 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::EvenMonitor>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 40 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::bus`
- 1 `(anonymous namespace)::monitor`
- 1 `(anonymous namespace)::controller`

</details>

## Case: multi_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 129.0 (+99.0) | 56 (+35) | 128 (+112) | 65 (+39) | 2/1 | 7228 (+4797) | 1112 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 39.0 (+9.0) | 29 (+8) | 16 (+0) | 34 (+8) | 0/0 | 2495 (+64) | 672 (+40) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 30.0 (+0.0) | 21 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 51.0 (+21.0) | 31 (+10) | 16 (+0) | 17 (-9) | 1/1 | 2627 (+196) | 672 (+40) | 0 | - | FAIL: publish instr, setup instr, no extra indirect calls, no extra RAM |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 104.0 (+95.0) | 56 (+35) | 128 (+112) | 65 (+59) | 2/1 | 7180 (+4813) | 1112 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 11.0 (+2.0) | 29 (+8) | 16 (+0) | 7 (+1) | 0/0 | 2415 (+48) | 672 (+40) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 9.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 24.0 (+15.0) | 31 (+10) | 16 (+0) | 17 (+11) | 1/1 | 2531 (+164) | 672 (+40) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |

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

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 112 `collapse_publish`
- 91 `collapse_setup`
- 24 `(anonymous namespace)::bus`
- 8 `(anonymous namespace)::sensor`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 105 `collapse_setup`
- 99 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 24 `(anonymous namespace)::bus`
- 16 `(anonymous namespace)::sensor`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 150.0 (+117.0) | 55 (+36) | 135 (+121) | 66 (+37) | 1/2 | 5023 (+2853) | 1137 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 33.0 (+0.0) | 19 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2170 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 44.0 (+11.0) | 27 (+8) | 14 (+0) | 41 (+12) | 1/0 | 2315 (+145) | 704 (+32) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 122.0 (+113.0) | 55 (+38) | 135 (+121) | 66 (+62) | 1/2 | 4980 (+2906) | 1137 (+473) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 9.0 (+0.0) | 19 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2090 (+16) | 672 (+8) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 17.0 (+8.0) | 27 (+10) | 14 (+0) | 14 (+10) | 1/0 | 2239 (+165) | 704 (+40) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |

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

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 87 `collapse_setup`
- 82 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerES6_NS1_6LoggerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSB_ENUlPKvRKS2_E_8__invokeESI_SK_`
- 24 `(anonymous namespace)::bus`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 4 `(anonymous namespace)::logger`
- 4 `(anonymous namespace)::controllerB`
- 4 `(anonymous namespace)::controllerA`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 162 (+132) | 4/1 | 2940 (+1752) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 31 (+1) | 0/0 | 1208 (+20) | 536 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (-15) | 0/1 | 1240 (+52) | 540 (+20) | 0 | - | FAIL: no extra indirect calls, no extra RAM |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 162 (+150) | 4/1 | 2900 (+1764) | 948 (+428) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 14 (+2) | 0/0 | 1160 (+24) | 536 (+16) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (+3) | 0/1 | 1184 (+48) | 540 (+20) | 0 | - | FAIL: publish path, no extra indirect calls, no extra RAM |

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

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 52 `collapse_setup`
- 12 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::sensor`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 68 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>, 0>(sub0x::Wiring<(anonymous namespace)::Controller, (anonymous namespace)::Controller, (anonymous namespace)::Logger>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 60 `collapse_setup`
- 12 `(anonymous namespace)::bus`
- 8 `(anonymous namespace)::sensor`

</details>

## Case: one_receiver

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 15.0 (+0.0) | 19 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 624 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 47.0 (+32.0) | 34 (+15) | 49 (+33) | 51 (+40) | 2/0 | 5048 (+2697) | 992 (+368) | 248 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 17.0 (+2.0) | 23 (+4) | 16 (+0) | 13 (+2) | 0/0 | 2383 (+32) | 640 (+16) | 0 | - | FAIL: publish instr, setup instr, no extra RAM |
| sub0x_b2_static | ok | 15.0 (+0.0) | 19 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 31.0 (+16.0) | 25 (+6) | 16 (+0) | 17 (+6) | 1/1 | 2515 (+164) | 648 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 19 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 624 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 8.0 (+0.0) | 34 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4716 (+2397) | 984 (+360) | 248 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 23 (+4) | 16 (+0) | 4 (+0) | 0/0 | 2351 (+32) | 640 (+16) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 8.0 (+0.0) | 19 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 624 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 22.0 (+14.0) | 25 (+6) | 16 (+0) | 17 (+13) | 1/1 | 2483 (+164) | 648 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |

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

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 46 `collapse_publish`
- 43 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 79 `collapse_publish`
- 57 `collapse_setup`
- 34 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 16 `(anonymous namespace)::sensor`
- 8 `(anonymous namespace)::bus`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 78.0 (+60.0) | 32 (+15) | 54 (+40) | 66 (+52) | 1/2 | 4442 (+2336) | 1033 (+369) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 18.0 (+0.0) | 17 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2106 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 25.0 (+7.0) | 21 (+4) | 14 (+0) | 22 (+8) | 1/0 | 2214 (+108) | 680 (+16) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 69.0 (+61.0) | 32 (+16) | 54 (+40) | 66 (+62) | 1/2 | 4426 (+2352) | 1033 (+377) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 8.0 (+0.0) | 21 (+5) | 14 (+0) | 4 (+0) | 0/0 | 2106 (+32) | 680 (+24) | 0 | - | FAIL: setup instr, no extra RAM |

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

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 39 `collapse_setup`
- 29 `_ZZN5sub0x4SinkIN12_GLOBAL__N_16SampleEEC1INS_6WiringIJNS1_10ControllerEEEETnNSt9enable_ifIXntsr3stdE9is_same_vINSt9remove_cvIT_E4typeES3_EEiE4typeELi0EEERSA_ENUlPKvRKS2_E_8__invokeESH_SJ_`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 8 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::controller`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 15 (+0) | 0/0 | 1128 (+0) | 512 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 146 (+131) | 2/0 | 2668 (+1540) | 660 (+148) | 64 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 17 (+2) | 0/0 | 1148 (+20) | 520 (+8) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 15 (+0) | 0/0 | 1128 (+0) | 512 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (+0) | 0/1 | 1176 (+48) | 524 (+12) | 0 | - | FAIL: no extra indirect calls, no extra RAM |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1104 (+0) | 512 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 7 (+0) | 0/0 | 2308 (+1204) | 660 (+148) | 64 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1120 (+16) | 520 (+8) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1104 (+0) | 512 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (+8) | 0/1 | 1152 (+48) | 524 (+12) | 0 | - | FAIL: publish path, no extra indirect calls, no extra RAM |

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

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 44 `collapse_publish`
- 28 `collapse_setup`
- 4 `(anonymous namespace)::sensor`
- 4 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 36 `collapse_setup`
- 28 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<(anonymous namespace)::Controller>, 0>(sub0x::Wiring<(anonymous namespace)::Controller>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 8 `(anonymous namespace)::sensor`
- 4 `(anonymous namespace)::bus`

</details>

## Case: transport_endpoint

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 30.0 (+3.0) | 24 (+6) | 16 (+0) | 31 (+8) | 0/0 | 2447 (+64) | 648 (+32) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 27.0 (+0.0) | 18 (+0) | 16 (+0) | 23 (+0) | 0/0 | 2383 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 205.0 (+178.0) | 62 (+44) | 124 (+108) | 115 (+92) | 3/2 | 5973 (+3590) | 1056 (+440) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 18.0 (+4.0) | 24 (+6) | 16 (+0) | 20 (+9) | 0/0 | 2415 (+64) | 648 (+32) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 14.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2351 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 189.0 (+175.0) | 62 (+44) | 124 (+108) | 115 (+104) | 3/2 | 5957 (+3606) | 1056 (+440) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 104 `collapse_publish`
- 47 `collapse_setup`
- 16 `(anonymous namespace)::bus`
- 8 `(anonymous namespace)::uplink`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 33.0 (+4.0) | 20 (+4) | 14 (+0) | 34 (+9) | 0/0 | 2186 (+48) | 680 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 29.0 (+0.0) | 16 (+0) | 14 (+0) | 25 (+0) | 0/0 | 2138 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 238.0 (+209.0) | 60 (+44) | 121 (+107) | 136 (+111) | 2/4 | 5425 (+3287) | 1048 (+392) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 18.0 (+4.0) | 20 (+4) | 14 (+0) | 20 (+10) | 0/0 | 2138 (+48) | 680 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 14.0 (+0.0) | 16 (+0) | 14 (+0) | 10 (+0) | 0/0 | 2090 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | ok | 220.0 (+206.0) | 60 (+44) | 121 (+107) | 136 (+126) | 2/4 | 5409 (+3319) | 1048 (+392) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>clang-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 107 `collapse_publish`
- 29 `collapse_setup`
- 8 `_ZN12_GLOBAL__N_13busE.0`
- 8 `(anonymous namespace)::uplink`
- 1 `(anonymous namespace)::radio`

</details>

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0 | - | reference |
| sub0x_b1_wire | - | - | - | - | 34 (+9) | 0/0 | 1200 (+52) | 524 (+16) | 0 | - | FAIL: publish path, no extra RAM |
| sub0x_b2_static | - | - | - | - | 25 (+0) | 0/0 | 1148 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | - | - | - | - | 20 (-5) | 0/2 | 3056 (+1908) | 928 (+420) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0 | - | reference |
| sub0x_b1_wire | - | - | - | - | 25 (+12) | 0/0 | 1172 (+60) | 524 (+16) | 0 | - | FAIL: publish path, no extra RAM |
| sub0x_b2_static | - | - | - | - | 13 (+0) | 0/0 | 1112 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_dynamic_route | - | - | - | - | 20 (+7) | 0/2 | 3036 (+1924) | 928 (+420) | 0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 92 `collapse_publish`
- 32 `collapse_setup`
- 8 `(anonymous namespace)::bus`
- 4 `(anonymous namespace)::uplink`
- 1 `(anonymous namespace)::radio`
- 1 `(anonymous namespace)::controller`

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

## Case: two_domains

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 31.0 (+0.0) | 21 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 41.0 (+10.0) | 31 (+10) | 16 (+0) | 37 (+10) | 0/0 | 2527 (+96) | 680 (+48) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra RAM |
| sub0x_b2_static | ok | 31.0 (+0.0) | 21 (+0) | 16 (+0) | 27 (+0) | 0/0 | 2431 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 178.0 (+147.0) | 107 (+86) | 233 (+217) | 128 (+101) | 3/2 | 6286 (+3855) | 1192 (+560) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 10.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 12.0 (+2.0) | 31 (+10) | 16 (+0) | 7 (+1) | 0/0 | 2431 (+64) | 680 (+48) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 10.0 (+0.0) | 21 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2367 (+0) | 632 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 153.0 (+143.0) | 107 (+86) | 233 (+217) | 128 (+122) | 3/2 | 6238 (+3871) | 1192 (+560) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 124 `collapse_publish`
- 105 `collapse_setup`
- 16 `(anonymous namespace)::busA`
- 8 `(anonymous namespace)::sensorB`
- 8 `(anonymous namespace)::sensorA`
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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 34.0 (+0.0) | 19 (+0) | 14 (+0) | 31 (+0) | 0/0 | 2186 (+0) | 672 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 203.0 (+169.0) | 79 (+60) | 195 (+181) | 138 (+107) | 2/4 | 5940 (+3754) | 1176 (+504) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | reference |
| sub0x_b1_wire | ok | 9.0 (+0.0) | 18 (+1) | 14 (+0) | 6 (+0) | 0/0 | 2106 (+16) | 664 (+0) | 0 | - | FAIL: setup instr |
| sub0x_b2_static | ok | 9.0 (+0.0) | 17 (+0) | 14 (+0) | 6 (+0) | 0/0 | 2090 (+0) | 664 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | ok | 175.0 (+166.0) | 79 (+62) | 195 (+181) | 138 (+132) | 2/4 | 5908 (+3818) | 1176 (+512) | 0 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no extra dependencies |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | reference |
| sub0x_b1_wire | - | - | - | - | 39 (+9) | 0/0 | 1240 (+52) | 540 (+20) | 0 | - | FAIL: publish path, no extra RAM |
| sub0x_b2_static | - | - | - | - | 30 (+0) | 0/0 | 1188 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | - | - | - | - | 23 (-7) | 0/2 | 3760 (+2572) | 1008 (+488) | 0 | TLS, operator delete | FAIL: no extra indirect calls, no extra RAM, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | reference |
| sub0x_b1_wire | - | - | - | - | 16 (+4) | 0/0 | 1180 (+44) | 540 (+20) | 0 | - | FAIL: publish path, no extra RAM |
| sub0x_b2_static | - | - | - | - | 12 (+0) | 0/0 | 1136 (+0) | 520 (+0) | 0 | - | PASS |
| sub0x_dynamic_domain | - | - | - | - | 23 (+11) | 0/2 | 3720 (+2584) | 1008 (+488) | 0 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no extra dependencies |

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 96 `collapse_publish`
- 68 `collapse_setup`
- 8 `(anonymous namespace)::busA`
- 4 `(anonymous namespace)::sensorB`
- 4 `(anonymous namespace)::sensorA`
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

## Case: zero_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 20 (+2) | 16 (+0) | 4 (+0) | 0/0 | 2335 (+16) | 632 (+16) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 22.0 (+14.0) | 22 (+4) | 16 (+0) | 17 (+13) | 1/1 | 2467 (+148) | 640 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 18.0 (+10.0) | 21 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3247 (+928) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 20 (+2) | 16 (+0) | 4 (+0) | 0/0 | 2335 (+16) | 632 (+16) | 0 | - | FAIL: setup instr, no extra RAM |
| sub0x_b2_static | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2319 (+0) | 616 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 22.0 (+14.0) | 22 (+4) | 16 (+0) | 17 (+13) | 1/1 | 2467 (+148) | 640 (+24) | 0 | - | FAIL: publish instr, setup instr, publish path, no extra indirect calls, no extra RAM |

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

<details><summary>gcc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 19 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::bus`

</details>

<details><summary>gcc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 79 `collapse_publish`
- 33 `collapse_setup`
- 16 `(anonymous namespace)::sensor`
- 5 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<>, 0>(sub0x::Wiring<>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 1 `(anonymous namespace)::bus`

</details>

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 8.0 (+0.0) | 18 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+16) | 0 | - | FAIL: setup instr, no extra RAM |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 8.0 (+0.0) | 19 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2830 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b2_static | ok | 8.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 656 (+0) | 0 | - | PASS |
| sub0x_b3_sink | ok | 8.0 (+0.0) | 18 (+2) | 14 (+0) | 4 (+0) | 0/0 | 2074 (+0) | 672 (+16) | 0 | - | FAIL: setup instr, no extra RAM |

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

<details><summary>clang-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 15 `collapse_setup`
- 8 `_ZN12_GLOBAL__N_16sensorE.0`
- 1 `(anonymous namespace)::bus`

</details>

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1108 (+12) | 516 (+8) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (+8) | 0/1 | 1140 (+44) | 520 (+12) | 0 | - | FAIL: publish path, no extra indirect calls, no extra RAM |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1816 (+720) | 916 (+408) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | - | - | - | - | 7 (+0) | 0/0 | 1108 (+12) | 516 (+8) | 0 | - | FAIL: no extra RAM |
| sub0x_b2_static | - | - | - | - | 7 (+0) | 0/0 | 1096 (+0) | 508 (+0) | 0 | - | PASS |
| sub0x_b3_sink | - | - | - | - | 15 (+8) | 0/1 | 1140 (+44) | 520 (+12) | 0 | - | FAIL: publish path, no extra indirect calls, no extra RAM |

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

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 16 `collapse_setup`
- 4 `(anonymous namespace)::sensor`
- 1 `(anonymous namespace)::bus`

</details>

<details><summary>cm33-gcc-Os: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 36 `collapse_publish`
- 24 `collapse_setup`
- 8 `(anonymous namespace)::sensor`
- 2 `sub0x::Sink<(anonymous namespace)::Sample>::Sink<sub0x::Wiring<>, 0>(sub0x::Wiring<>&)::{lambda(void const*, (anonymous namespace)::Sample const&)#1}::_FUN(void const*, (anonymous namespace)::Sample const&)`
- 1 `(anonymous namespace)::bus`

</details>


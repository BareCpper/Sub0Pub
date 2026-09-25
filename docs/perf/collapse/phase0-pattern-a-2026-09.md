# Collapse evidence (issue #9)

Final-link evidence per case, build and form; every variant is compared with `handwritten` (equal-work reference, same build and form). Deltas in parentheses. instr = callgrind instructions (publish: per publication of 1000). path = static instructions of `collapse_publish` plus directly reachable functions.

- **gcc-O2**: `g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` `-O2`
- **clang-O2**: `Ubuntu clang version 18.1.3 (1ubuntu1)` `-O2`
- **cm33-gcc-Os**: `arm-none-eabi-g++ (15:13.2.rel1-2) 13.2.1 20231009` `-Os -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fno-exceptions -fno-rtti -DCOLLAPSE_NO_STDIO`

## Case: filters

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 19.0 (+0.0) | 17 (+0) | 16 (+0) | 18 (+0) | 0/0 | 2345 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 122.5 (+103.5) | 43 (+26) | 89 (+73) | 73 (+55) | 2/2 | 6696 (+4351) | 1096 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 17 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2297 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 111.0 (+103.0) | 43 (+26) | 89 (+73) | 73 (+69) | 2/2 | 6620 (+4323) | 1096 (+480) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 22.0 (+0.0) | 15 (+0) | 14 (+0) | 19 (+0) | 0/0 | 2100 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 113.0 (+91.0) | 41 (+26) | 92 (+78) | 66 (+47) | 1/2 | 4915 (+2815) | 1121 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2052 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 99.5 (+91.5) | 41 (+26) | 92 (+78) | 66 (+62) | 1/2 | 4883 (+2831) | 1121 (+465) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 19 (+0) | 0/0 | 1116 (+0) | 504 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 168 (+149) | 4/2 | 2920 (+1804) | 924 (+420) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1084 (+0) | 504 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 168 (+161) | 4/2 | 2872 (+1788) | 924 (+420) | 225 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

## Case: multi_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 30.0 (+0.0) | 20 (+0) | 16 (+0) | 26 (+0) | 0/0 | 2409 (+0) | 624 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 129.0 (+99.0) | 55 (+35) | 128 (+112) | 65 (+39) | 2/1 | 7180 (+4771) | 1112 (+488) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 20 (+0) | 16 (+0) | 6 (+0) | 0/0 | 2345 (+0) | 624 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 104.0 (+95.0) | 55 (+35) | 128 (+112) | 65 (+59) | 2/1 | 7132 (+4787) | 1112 (+488) | 257 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 33.0 (+0.0) | 18 (+0) | 14 (+0) | 29 (+0) | 0/0 | 2148 (+0) | 664 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 150.0 (+117.0) | 54 (+36) | 135 (+121) | 66 (+37) | 1/2 | 5001 (+2853) | 1137 (+473) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 9.0 (+0.0) | 16 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2052 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 122.0 (+113.0) | 54 (+38) | 135 (+121) | 66 (+62) | 1/2 | 4958 (+2906) | 1137 (+481) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 30 (+0) | 0/0 | 1176 (+0) | 516 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 162 (+132) | 4/1 | 2928 (+1752) | 948 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 12 (+0) | 0/0 | 1124 (+0) | 516 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 162 (+150) | 4/1 | 2888 (+1764) | 948 (+432) | 265 | TLS, operator delete | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

## Case: one_receiver

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 15.0 (+0.0) | 18 (+0) | 16 (+0) | 11 (+0) | 0/0 | 2329 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 47.0 (+32.0) | 33 (+15) | 49 (+33) | 51 (+40) | 2/0 | 5000 (+2671) | 992 (+376) | 248 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 18 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2297 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 8.0 (+0.0) | 33 (+15) | 49 (+33) | 4 (+0) | 0/0 | 4668 (+2371) | 984 (+368) | 248 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 18.0 (+0.0) | 16 (+0) | 14 (+0) | 14 (+0) | 0/0 | 2084 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 78.0 (+60.0) | 31 (+15) | 54 (+40) | 66 (+52) | 1/2 | 4420 (+2336) | 1033 (+377) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2052 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 69.0 (+61.0) | 31 (+16) | 54 (+40) | 66 (+62) | 1/2 | 4404 (+2352) | 1033 (+377) | 596 | operator delete | FAIL: publish instr, setup instr, teardown instr, publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 15 (+0) | 0/0 | 1116 (+0) | 508 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 146 (+131) | 2/0 | 2656 (+1540) | 656 (+148) | 64 | operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1092 (+0) | 508 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 7 (+0) | 0/0 | 2296 (+1204) | 656 (+148) | 64 | operator delete | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |

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

## Case: zero_receivers

### gcc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 17 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2297 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 18.0 (+10.0) | 20 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3199 (+902) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### gcc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 17 (+0) | 16 (+0) | 4 (+0) | 0/0 | 2297 (+0) | 616 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 18.0 (+10.0) | 20 (+3) | 16 (+0) | 14 (+10) | 0/0 | 3199 (+902) | 848 (+232) | 139 | operator delete | FAIL: publish instr, setup instr, publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### clang-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2052 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 8.0 (+0.0) | 18 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2808 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |

### clang-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | 8.0 (+0.0) | 15 (+0) | 14 (+0) | 4 (+0) | 0/0 | 2052 (+0) | 656 (+0) | 0 | - | reference |
| sub0pub_virtual | ok | 8.0 (+0.0) | 18 (+3) | 19 (+5) | 4 (+0) | 0/0 | 2808 (+756) | 784 (+128) | 105 | operator delete | FAIL: setup instr, teardown instr, no extra RAM, no Sub0Pub retained, no extra dependencies |

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

### cm33-gcc-Os, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1084 (+0) | 504 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1804 (+720) | 908 (+404) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### cm33-gcc-Os, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0 (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | - | - | - | - | 7 (+0) | 0/0 | 1084 (+0) | 504 (+0) | 0 | - | reference |
| sub0pub_virtual | - | - | - | - | 30 (+23) | 1/0 | 1804 (+720) | 908 (+404) | 41 | TLS, operator delete | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

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


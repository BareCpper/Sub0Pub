# Sub0Pub footprint report

- **host**: `g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0` `-std=c++17 -Os -fno-exceptions -fno-rtti`
- **cm33**: `arm-none-eabi-g++ (15:13.2.rel1-2) 13.2.1 20231009` `-std=c++17 -Os -fno-exceptions -fno-rtti -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16`

## Target: host

### Object size (text / data / bss bytes)

| Scenario | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| 1 type: 1 publisher, 1 subscriber, 1 publish site | 1080 / 136 / 145 | 969 / 136 / 145 | 1026 / 136 / 145 | 1270 / 136 / 185 |
| +1 subscriber of the same type | 1171 / 136 / 129 | 1052 / 136 / 129 | 1104 / 136 / 129 | 1344 / 136 / 169 |
| +1 publish call site of the same type | 1177 / 136 / 113 | 1066 / 136 / 113 | 1123 / 136 / 113 | 1367 / 136 / 153 |
| +1 Data type (publisher, subscriber, site) | 2075 / 264 / 226 | 1845 / 264 / 226 | 1958 / 264 / 226 | 2407 / 264 / 306 |
| 1 type forwarded to StreamSerializer | 1384 / 136 / 129 | 1273 / 136 / 129 | 1330 / 136 / 129 | 1574 / 136 / 169 |

### Marginal text bytes

| Added usage | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| +1 subscriber of the same type | +91 | +83 | +78 | +74 |
| +1 publish call site of the same type | +97 | +97 | +97 | +97 |
| +1 Data type (publisher, subscriber, site) | +995 | +876 | +932 | +1137 |

### Sub0Pub symbols, 1 type (bytes)

| Symbol | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| `Broker<MsgA>::threadCanceled_` | 1 | 1 | 1 | 1 |
| `Subscribe<MsgA>::filter()` | 7 | 7 | 7 | 7 |
| `Broker<MsgA>::threadCurrent_` | 8 | 8 | 8 | 8 |
| `vtable for Subscribe<MsgA>` | 48 | 48 | 48 | 48 |
| `Broker<MsgA>::state_` | 72 | 72 | 72 | 112 |
| `Broker<MsgA>::unsubscribe()` | 191 | 191 | 209 | 226 |
| `Broker<MsgA>::publish()` | 244 | 141 | 149 | 264 |

### sizeof (bytes)

- `Publish<T>`: 16
- `Subscribe<T>`: 16

### Link-time dependencies, 1 type (undefined symbols)

- **Snapshot (default)**: `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)`
- **Direct unchecked**: `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)`
- **Direct + check**: `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `abort`, `memmove`, `operator delete(void*, unsigned long)`
- **ThreadSafe**: `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `std::__throw_system_error(int)`

## Target: cm33

### Object size (text / data / bss bytes)

| Scenario | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| 1 type: 1 publisher, 1 subscriber, 1 publish site | 566 / 4 / 77 | 526 / 4 / 77 | 574 / 4 / 77 | n/a |
| +1 subscriber of the same type | 610 / 4 / 73 | 570 / 4 / 73 | 614 / 4 / 73 | n/a |
| +1 publish call site of the same type | 590 / 4 / 61 | 550 / 4 / 61 | 598 / 4 / 61 | n/a |
| +1 Data type (publisher, subscriber, site) | 1120 / 4 / 122 | 1040 / 4 / 122 | 1132 / 4 / 122 | n/a |
| 1 type forwarded to StreamSerializer | 704 / 4 / 65 | 664 / 4 / 65 | 708 / 4 / 65 | n/a |

### Marginal text bytes

| Added usage | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| +1 subscriber of the same type | +44 | +44 | +40 | n/a |
| +1 publish call site of the same type | +24 | +24 | +24 | n/a |
| +1 Data type (publisher, subscriber, site) | +554 | +514 | +558 | n/a |

### Sub0Pub symbols, 1 type (bytes)

| Symbol | Snapshot (default) | Direct unchecked | Direct + check | ThreadSafe |
|---|---:|---:|---:|---:|
| `Broker<MsgA>::threadCanceled_` | 1 | 1 | 1 | - |
| `Broker<MsgA>::threadCurrent_` | 4 | 4 | 4 | - |
| `Subscribe<MsgA>::filter()` | 4 | 4 | 4 | - |
| `vtable for Subscribe<MsgA>` | 24 | 24 | 24 | - |
| `Broker<MsgA>::state_` | 36 | 36 | 36 | - |
| `Broker<MsgA>::publish()` | 152 | 112 | 112 | - |
| `Broker<MsgA>::unsubscribe()` | 156 | 156 | 176 | - |

### sizeof (bytes)

- `Publish<T>`: 8
- `Subscribe<T>`: 8

### Link-time dependencies, 1 type (undefined symbols)

- **Snapshot (default)**: `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)`
- **Direct unchecked**: `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memmove`, `operator delete(void*, unsigned int)`
- **Direct + check**: `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `abort`, `memmove`, `operator delete(void*, unsigned int)`
- **ThreadSafe**: does not compile: `/home/user/Sub0Pub/include/sub0pub/sub0pub.hpp:620:26: error: 'mutex' in namespace 'std' does not name a type`

## Prototype: sub0x per-type configurations

Same usage as `fp_1type` (1 type, 1 publisher, 1 subscriber, 1 publish site); compare with the Snapshot (default) column above.

### Target: host

| Configuration | text / data / bss | `Broker::publish()` | sizeof Subscribe / Publish | Link-time dependencies |
|---|---:|---:|---:|---|
| Default (Snapshot, ThreadLocal, filter) | 994 / 104 / 114 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Direct | 941 / 104 / 114 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Direct + StaticContext (no TLS) | 933 / 104 / 114 | inlined | 16 / 1 | `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Lean (Direct, NoContext, NoFilter) | 696 / 88 / 106 | inlined | 16 / 1 | `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, Dispatch=DirectChecked | 1103 / 104 / 114 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `abort`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, Context=Static | 986 / 104 / 114 | inlined | 16 / 1 | `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, Context=None (must be rejected: Snapshot needs a context) | n/a: `/home/user/Sub0Pub/tests/design/broker_config/sub0x_broker.hpp:609:68: error: static assertion failed: sub0x: Snapshot needs a publish context (StaticContext or ThreadLocalContext): a subscriber disconnected during a dispatch is removed from that dispatch's snapshot through its frame` | | | |
| Default, Filter=off | 935 / 88 / 114 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, Lock=spin (RTOS-style yield hook) | 1459 / 104 / 130 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)`, `pthread_self` |
| Default, Lock=spin + StaticContext (must be rejected) | n/a: `/home/user/Sub0Pub/tests/design/broker_config/sub0x_broker.hpp:614:48: error: static assertion failed: sub0x: a Lock requires ThreadLocalContext (disconnect must not wait on its own dispatch, and a StaticContext frame stack shared by concurrent publishers lets one thread's cancel() and frames act on another thread's dispatch)` | | | |
| Default, Storage=Scoped | 1561 / 104 / 232 | inlined | 24 / 8, Domain 80 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `abort`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, Capacity=64 | 1092 / 104 / 562 | 282 | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, Implementation=SingleSubscriberBroker | 716 / 104 / 50 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `operator delete(void*, unsigned long)` |
| Default + 1 Route | 1336 / 152 / 166 | inlined | 16 / 1, Route 24 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Default, 2 Data types (marginal cost of a type) | 1921 / 200 / 211 | inlined | 16 / 1 | `_GLOBAL_OFFSET_TABLE_`, `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |
| Lean, 2 Data types (marginal cost of a type) | 1317 / 168 / 195 | inlined | 16 / 1 | `__cxa_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `__stack_chk_fail`, `memmove`, `operator delete(void*, unsigned long)` |

### Target: cm33

| Configuration | text / data / bss | `Broker::publish()` | sizeof Subscribe / Publish | Link-time dependencies |
|---|---:|---:|---:|---|
| Default (Snapshot, ThreadLocal, filter) | 570 / 4 / 62 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)` |
| Direct | 538 / 4 / 62 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memmove`, `operator delete(void*, unsigned int)` |
| Direct + StaticContext (no TLS) | 526 / 4 / 62 | inlined | 8 / 1 | `__aeabi_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `memmove`, `operator delete(void*, unsigned int)` |
| Lean (Direct, NoContext, NoFilter) | 394 / 4 / 58 | inlined | 8 / 1 | `__aeabi_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, Dispatch=DirectChecked | 598 / 4 / 62 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `abort`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, Context=Static | 558 / 4 / 62 | inlined | 8 / 1 | `__aeabi_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, Context=None (must be rejected: Snapshot needs a context) | n/a: `/home/user/Sub0Pub/tests/design/broker_config/sub0x_broker.hpp:609:68: error: static assertion failed: sub0x: Snapshot needs a publish context (StaticContext or ThreadLocalContext): a subscriber disconnected during a dispatch is removed from that dispatch's snapshot through its frame` | | | |
| Default, Filter=off | 530 / 4 / 62 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, Lock=spin (RTOS-style yield hook) | 762 / 4 / 70 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memmove`, `memset`, `operator delete(void*, unsigned int)` |
| Default, Lock=spin + StaticContext (must be rejected) | n/a: `/home/user/Sub0Pub/tests/design/broker_config/sub0x_broker.hpp:614:48: error: static assertion failed: sub0x: a Lock requires ThreadLocalContext (disconnect must not wait on its own dispatch, and a StaticContext frame stack shared by concurrent publishers lets one thread's cancel() and frames act on another thread's dispatch)` | | | |
| Default, Storage=Scoped | 844 / 4 / 128 | inlined | 12 / 4, Domain 44 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `abort`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, Capacity=64 | 606 / 4 / 286 | 164 | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, Implementation=SingleSubscriberBroker | 382 / 4 / 30 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `operator delete(void*, unsigned int)` |
| Default + 1 Route | 758 / 4 / 90 | inlined | 8 / 1, Route 12 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)` |
| Default, 2 Data types (marginal cost of a type) | 1128 / 4 / 115 | inlined | 8 / 1 | `__aeabi_atexit`, `__aeabi_read_tp`, `__cxa_pure_virtual`, `__dso_handle`, `memcpy`, `memmove`, `operator delete(void*, unsigned int)` |
| Lean, 2 Data types (marginal cost of a type) | 776 / 4 / 107 | inlined | 8 / 1 | `__aeabi_atexit`, `__cxa_pure_virtual`, `__dso_handle`, `memmove`, `operator delete(void*, unsigned int)` |


# Collapse evidence (issue #9)

Final-link evidence per case, build and form; every variant is compared with `handwritten` (equal-work reference, same build and form), or with the extra reference it names, shown as `variant (vs handwritten_<kind>)`: e.g. `handwritten_runtime`, hand-written code that reaches its receivers through addresses stored at setup. Deltas in parentheses. instr = callgrind instructions (publish: per publication of 1000). path = static instructions of `collapse_publish` plus directly reachable functions.

- **msvc-O2**: `Microsoft (R) C/C++ Optimizing Compiler Version 19.51.36246 for x64` `/O2`

## Case: cancellation

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 21 (+0) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 24 (+3) | 0/0 | 136020 (+32) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_alt1_bool | ok | - | - | - | 22 (+1) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | - | - | - | 24 (+0) | 0/0 | 136020 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | - | - | - | 22 (+1) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | - | - | - | 22 (+1) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | - | - | - | 22 (+1) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | - | - | - | 36 (+15) | 0/0 | 136120 (+132) | 5696 (+8) | 0/1 | TLS | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_alt4_filter | ok | - | - | - | 25 (+4) | 0/0 | 136004 (+16) | 5688 (+0) | 0/0 | - | FAIL: publish path |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 9 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 10 (+1) | 0/0 | 135956 (+32) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_alt1_bool | ok | - | - | - | 9 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | - | - | - | 10 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | - | - | - | 9 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | - | - | - | 9 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | - | - | - | 9 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | - | - | - | 19 (+10) | 0/0 | 136028 (+104) | 5696 (+8) | 0/1 | TLS | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_alt4_filter | ok | - | - | - | 10 (+1) | 0/0 | 135940 (+16) | 5688 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 64 `collapse_setup`
- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Gate> `anonymous namespace'::gate`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt1_bool (bytes)</summary>

- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt1_bool_b1 (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Gate,struct A0x5b547aa6::Controller,struct A0x5b547aa6::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Gate> `anonymous namespace'::gate`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt1c_expected_cpp23 (bytes)</summary>

- 1188 `__volatile_metadata`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt2_token (bytes)</summary>

- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt3_static (bytes)</summary>

- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt3_tls (bytes)</summary>

- 1292 `__volatile_metadata`
- 160 `collapse_publish`
- 40 `_tls_used`
- 24 `$chain$2$__isa_available_init`
- 12 `$unwind$collapse_publish`
- 8 `_tls_index`
- 8 `__xl_z`
- 8 `__xl_a`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt4_filter (bytes)</summary>

- 112 `collapse_publish`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Stopped> `anonymous namespace'::stopped`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

## Case: cancellation_filtered

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 26 (+0) | 0/0 | 136004 (+0) | 5688 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | - | - | - | 30 (+4) | 0/0 | 136020 (+16) | 5688 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_alt2_token | ok | - | - | - | 30 (+4) | 0/0 | 136020 (+16) | 5688 (+0) | 0/0 | - | FAIL: publish path |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 11 (+0) | 0/0 | 135940 (+0) | 5688 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | - | - | - | 11 (+0) | 0/0 | 135940 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | - | - | - | 11 (+0) | 0/0 | 135940 (+0) | 5688 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2: largest symbols added by sub0x_alt1_bool (bytes)</summary>

- 128 `collapse_publish`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_alt2_token (bytes)</summary>

- 128 `collapse_publish`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

## Case: cross_file

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 27 (+0) | 3/0 | 136056 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 13 (-14) | 1/1 | 148556 (+12500) | 5736 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 27 (+0) | 3/0 | 136104 (+48) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+32) | 1/2 | 147804 (+11748) | 6112 (+424) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+24) | 0/2 | 147772 (+11716) | 6112 (+424) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 27 (+0) | 3/0 | 136104 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 27 (+0) | 3/0 | 136056 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 13 (+0) | 1/1 | 148564 (+8) | 5736 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 17 (+0) | 3/0 | 136024 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 13 (-4) | 1/1 | 148524 (+12500) | 5736 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 17 (+0) | 3/0 | 136072 (+48) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+42) | 1/2 | 147772 (+11748) | 6112 (+424) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+34) | 0/2 | 147740 (+11716) | 6112 (+424) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 17 (+0) | 3/0 | 136072 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 17 (+0) | 3/0 | 136024 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 13 (+0) | 1/1 | 148532 (+8) | 5736 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1212 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1180 `__volatile_metadata`
- 80 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `collapse_publish`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct app::Controller,struct app::Controller,struct app::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 1180 `__volatile_metadata`
- 8 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 160 `struct _EXCEPTION_RECORD const `public: static void __cdecl __FrameHandler3::UnwindNestedFrames(unsigned __int64 * __ptr64,struct EHExceptionRecord * __ptr64,struct _CONTEXT * __ptr64,unsigned __int64 * __ptr64,void * __ptr64,struct _s_FuncInfo const * __ptr64,int,int,struct _s_HandlerType const * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,unsigned char)'::`2'::ExceptionTemplate`
- 64 `private: static __cdecl <lambda_5388d9f02801c667922566f057c90ec7>::<lambda_invoker_cdecl>(void const * __ptr64,struct app::Sample const & __ptr64)`
- 24 `unsigned char const * const FH4::s_shiftTab`
- 24 `class collapse::Slot<class sub0x::Wiring<struct app::Controller,struct app::Controller,struct app::Logger> > `anonymous namespace'::bus`
- 16 `class collapse::Slot<struct app::Sensor> `anonymous namespace'::sensor`
- 12 `$unwind$?<lambda_invoker_cdecl>@<lambda_5388d9f02801c667922566f057c90ec7>@@CA@PEBXAEBUSample@app@@@Z`
- 9 `$cppxdata$?send@Sensor@app@@QEAAXI@Z`
- 8 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`

</details>

## Case: dynamic_subscriptions

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 80 (+0) | 0/2 | 136776 (+0) | 5896 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | - | - | - | 113 (+33) | 5/2 | 147936 (+11160) | 6088 (+192) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 104 (+24) | 4/2 | 147904 (+11128) | 6088 (+192) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | - | - | - | 172 (+92) | 6/2 | 147652 (+10876) | 6064 (+168) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 86 (+6) | 2/2 | 147336 (+10560) | 6024 (+128) | 0/664 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 80 (+0) | 0/2 | 136744 (+0) | 5896 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | - | - | - | 113 (+33) | 5/2 | 147904 (+11160) | 6088 (+192) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 104 (+24) | 4/2 | 147872 (+11128) | 6088 (+192) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | - | - | - | 172 (+92) | 6/2 | 147620 (+10876) | 6064 (+168) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 86 (+6) | 2/2 | 147304 (+10560) | 6024 (+128) | 0/664 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1308 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `public: void __cdecl `anonymous namespace'::Sensor::send(struct A0x671d2243::Sample const & __ptr64) __ptr64`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 152 `SetSmallXmm`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1308 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `public: void __cdecl `anonymous namespace'::Sensor::send(struct A0x06b8b729::Sample const & __ptr64) __ptr64`
- 152 `SetSmallXmm`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 288 `public: void __cdecl `anonymous namespace'::Sensor::send(struct A0xc53b554f::Sample const & __ptr64) __ptr64`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::~Subscribe<struct `anonymous namespace'::Sample>(void) __ptr64`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1196 `__volatile_metadata`
- 336 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

## Case: filters

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 12 (+0) | 0/0 | 135924 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-3) | 0/1 | 148400 (+12476) | 5720 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 12 (+0) | 0/0 | 135940 (+16) | 5704 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+47) | 1/2 | 147708 (+11784) | 6104 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+39) | 0/2 | 147676 (+11752) | 6104 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 14 (+2) | 0/0 | 135956 (+16) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 14 (+2) | 0/0 | 135940 (+16) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 14 (+2) | 0/0 | 135940 (+16) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148416 (+16) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+57) | 1/2 | 147612 (+11688) | 6080 (+408) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 32 (+20) | 0/2 | 147236 (+11312) | 6040 (+368) | 0/688 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 148368 (+12476) | 5720 (+48) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 2 (+0) | 0/0 | 135908 (+16) | 5704 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 147676 (+11784) | 6104 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 147644 (+11752) | 6104 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 135908 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148368 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 147580 (+11688) | 6080 (+408) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 32 (+30) | 0/2 | 147204 (+11312) | 6040 (+368) | 0/688 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1212 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1180 `__volatile_metadata`
- 32 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 7 `class collapse::Slot<struct `anonymous namespace'::EvenMonitor> `anonymous namespace'::monitor`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 160 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `collapse_publish`
- 160 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 64 `collapse_publish`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xf8180863::EvenMonitor> > > `anonymous namespace'::sensor`
- 7 `class collapse::Slot<struct `anonymous namespace'::EvenMonitor> `anonymous namespace'::monitor`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 64 `collapse_publish`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static_cxx20 (bytes)</summary>

- 1180 `__volatile_metadata`
- 64 `collapse_publish`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 64 `private: static __cdecl <lambda_5205c36cc0a0c2a5667b6897cce70fb2>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 16 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x67ec7764::EvenMonitor> > `anonymous namespace'::bus`
- 7 `class collapse::Slot<struct `anonymous namespace'::EvenMonitor> `anonymous namespace'::monitor`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 288 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::~Subscribe<struct `anonymous namespace'::Sample>(void) __ptr64`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1180 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 208 `collapse_setup`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

## Case: large_payload

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 26 (+0) | 0/0 | 136004 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (-4) | 0/1 | 148512 (+12508) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 28 (+2) | 0/0 | 136036 (+32) | 5704 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 72 (+46) | 1/2 | 147804 (+11800) | 6120 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 64 (+38) | 0/2 | 147772 (+11768) | 6120 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 28 (+0) | 0/0 | 136036 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 26 (+0) | 0/0 | 136004 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/1 | 148512 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 84 (+58) | 1/2 | 147872 (+11868) | 6080 (+392) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 33 (+7) | 0/1 | 147372 (+11368) | 6056 (+368) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (+19) | 0/1 | 148464 (+12572) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 135924 (+32) | 5704 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 72 (+69) | 1/2 | 147772 (+11880) | 6120 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 64 (+61) | 0/2 | 147740 (+11848) | 6120 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135924 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/1 | 148464 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 84 (+81) | 1/2 | 147840 (+11948) | 6080 (+392) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 33 (+30) | 0/1 | 147340 (+11448) | 6056 (+368) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1220 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 48 `collapse_setup`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 288 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 208 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 208 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x6e02330f::Logger> > > `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 64 `private: static __cdecl <lambda_6c293e47edef44e4cb475f3092774e43>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Frame const & __ptr64)`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 16 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xf1f64c08::Logger> > `anonymous namespace'::bus`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 384 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Frame>::~Subscribe<struct `anonymous namespace'::Frame>(void) __ptr64`
- 224 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1188 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_setup`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 160 `collapse_publish`

</details>

## Case: many_receivers

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 69 (+0) | 0/0 | 136580 (+0) | 5800 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-60) | 0/1 | 149908 (+13328) | 6072 (+272) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_loop | ok | - | - | - | 14 (-55) | 0/0 | 136276 (-304) | 5800 (+0) | 0/0 | - | reference; PASS |
| handwritten_runtime | ok | - | - | - | 197 (+128) | 1/0 | 137456 (+876) | 6056 (+256) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 60 (-9) | 1/2 | 149560 (+12980) | 6984 (+1184) | 968/0 | TLS, pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (-18) | 0/2 | 149512 (+12932) | 6984 (+1184) | 968/0 | TLS, pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 197 (+0) | 1/0 | 137760 (+304) | 6056 (+0) | 0/1104 | - | FAIL: no Sub0Pub retained |
| sub0x_b2_static | ok | - | - | - | 71 (+2) | 1/0 | 136628 (+48) | 5816 (+16) | 0/400 | - | FAIL: no extra RAM, no Sub0Pub retained |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 149840 (-68) | 6072 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+0) | 1/2 | 150132 (+13552) | 6960 (+1160) | 0/1000 | TLS, pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (-49) | 0/1 | 149676 (+13096) | 6920 (+1120) | 0/856 | pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 136212 (+0) | 5800 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 149156 (+12944) | 6072 (+272) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_loop | ok | - | - | - | 7 (+5) | 0/0 | 136244 (+32) | 5800 (+0) | 0/0 | - | reference; FAIL: publish path |
| handwritten_runtime | ok | - | - | - | 2 (+0) | 0/0 | 136704 (+492) | 6056 (+256) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 60 (+58) | 1/2 | 149544 (+13332) | 6984 (+1184) | 968/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 149496 (+13284) | 6984 (+1184) | 968/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 137008 (+304) | 6056 (+0) | 0/368 | - | FAIL: no Sub0Pub retained |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 136212 (+0) | 5800 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 149104 (-52) | 6072 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 150116 (+13904) | 6960 (+1160) | 0/1000 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 149660 (+13448) | 6920 (+1120) | 0/856 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1220 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 832 `collapse_setup`
- 752 `public: void __cdecl `anonymous namespace'::Node::deliver(struct A0x9b5770ab::Sample const & __ptr64)const __ptr64`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_loop (bytes)</summary>

- 128 `class collapse::Slot<struct `anonymous namespace'::Controller> * `anonymous namespace'::controllers`
- 32 `collapse_teardown`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 800 `collapse_setup`
- 736 `public: void __cdecl `anonymous namespace'::Sensor::send(unsigned int) __ptr64`
- 256 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 24 `$chain$2$__isa_available_init`
- 12 `$unwind$collapse_setup`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c9`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c8`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 2032 `collapse_setup`
- 1300 `__volatile_metadata`
- 416 `collapse_teardown`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 264 `private: static struct sub0::detail::Broker<struct `anonymous namespace'::Sample>::State sub0::detail::Broker<struct `anonymous namespace'::Sample>::state_`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 2032 `collapse_setup`
- 1300 `__volatile_metadata`
- 416 `collapse_teardown`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 264 `private: static struct sub0::detail::Broker<struct `anonymous namespace'::Sample>::State sub0::detail::Broker<struct `anonymous namespace'::Sample>::state_`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 736 `public: void __cdecl `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller> >::send(unsigned int) __ptr64`
- 368 `public: struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller> > & __ptr64 __cdecl collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller> > >::emplace<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller> >(class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller> && __ptr64) __ptr64`
- 256 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller,struct A0x5ba50ed4::Controller> > > `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c9`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c8`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c7`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c6`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c5`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 400 `public: void __cdecl `anonymous namespace'::Sensor<struct sub0x::StaticWiring<&class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c0,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c1,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c2,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c3,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c4,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c5,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c6,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c7,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c8,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c9,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c10,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c11,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c12,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c13,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c14,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c15,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c16,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c17,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c18,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c19,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c20,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c21,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c22,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c23,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c24,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c25,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c26,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c27,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c28,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c29,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c30,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c31> >::send(unsigned int) __ptr64`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 8 `class collapse::Slot<struct `anonymous namespace'::Sensor<struct sub0x::StaticWiring<&class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c0,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c1,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c2,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c3,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c4,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c5,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c6,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c7,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c8,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c9,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c10,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c11,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c12,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c13,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c14,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c15,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c16,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c17,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c18,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c19,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c20,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c21,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c22,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c23,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c24,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c25,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c26,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c27,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c28,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c29,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c30,&class collapse::Slot<struct `anonymous namespace'::Controller> A0xc71eff26::c31> > > `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c9`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c8`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c7`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c6`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c5`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 736 `public: __cdecl <lambda_67aff4e70aa4d7d015ee7631947ff1e7>::operator()(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)const __ptr64`
- 256 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller,struct A0xc45171d3::Controller> > `anonymous namespace'::bus`
- 24 `$chain$2$__isa_available_init`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 16 `private: static __cdecl <lambda_67aff4e70aa4d7d015ee7631947ff1e7>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c9`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c8`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::c7`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2528 `collapse_setup`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 400 `collapse_teardown`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 264 `private: static struct sub0x::detail::Table<struct `anonymous namespace'::Sample,struct sub0x::config<struct sub0x::Capacity<32> > > sub0x::detail::Broker<struct `anonymous namespace'::Sample,struct sub0x::config<struct sub0x::Capacity<32> > >::global_`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2528 `collapse_setup`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1188 `__volatile_metadata`
- 400 `collapse_teardown`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 264 `private: static struct sub0x::detail::Table<struct `anonymous namespace'::Sample,struct sub0x::config<struct sub0x::DispatchWith<1>,struct sub0x::ContextWith<2>,struct sub0x::NoFilter,struct sub0x::Capacity<32> > > sub0x::detail::Broker<struct `anonymous namespace'::Sample,struct sub0x::config<struct sub0x::DispatchWith<1>,struct sub0x::ContextWith<2>,struct sub0x::NoFilter,struct sub0x::Capacity<32> > >::global_`

</details>

## Case: multi_receivers

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-4) | 0/1 | 148480 (+12524) | 5736 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 22 (+9) | 0/0 | 136036 (+80) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+46) | 1/2 | 147804 (+11848) | 6152 (+464) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+46) | 1/2 | 147804 (+11848) | 6152 (+464) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+38) | 0/2 | 147772 (+11816) | 6152 (+464) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148480 (+0) | 5736 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+56) | 1/2 | 147708 (+11752) | 6112 (+424) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+7) | 0/1 | 147260 (+11304) | 6088 (+400) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+6) | 0/1 | 148416 (+12508) | 5736 (+48) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 135956 (+48) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+56) | 1/2 | 147772 (+11864) | 6152 (+464) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+56) | 1/2 | 147772 (+11864) | 6152 (+464) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+48) | 0/2 | 147740 (+11832) | 6152 (+464) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148416 (+0) | 5736 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+66) | 1/2 | 147676 (+11768) | 6112 (+424) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+17) | 0/1 | 147228 (+11320) | 6088 (+400) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1220 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 96 `collapse_publish`
- 80 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1308 `__volatile_metadata`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `collapse_publish`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x9c87c323::Controller,struct A0x9c87c323::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static_cxx20 (bytes)</summary>

- 1188 `__volatile_metadata`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 80 `private: static __cdecl <lambda_61fe93fe0e66fee1953af6ade1592ef3>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x0373bc24::Controller,struct A0x0373bc24::Logger> > `anonymous namespace'::bus`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 304 `collapse_setup`
- 288 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::~Subscribe<struct `anonymous namespace'::Sample>(void) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1188 `__volatile_metadata`
- 304 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

## Case: multi_types

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 15 (+0) | 0/0 | 135940 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 17 (+2) | 0/2 | 148544 (+12604) | 5736 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 19 (+4) | 0/0 | 136004 (+64) | 5704 (+16) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 101 (+86) | 2/4 | 149332 (+13392) | 6432 (+744) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 82 (+67) | 0/4 | 149252 (+13312) | 6432 (+744) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 19 (+0) | 0/0 | 136004 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 15 (+0) | 0/0 | 135940 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 18 (+1) | 0/2 | 148544 (+0) | 5736 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 131 (+116) | 3/4 | 149416 (+13476) | 6408 (+720) | 0/1856 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 34 (+19) | 0/2 | 148528 (+12588) | 6376 (+688) | 0/1552 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 17 (+14) | 0/2 | 148464 (+12572) | 5736 (+48) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 135940 (+48) | 5704 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 101 (+98) | 2/4 | 149268 (+13376) | 6432 (+744) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 82 (+79) | 0/4 | 149188 (+13296) | 6432 (+744) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135940 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 18 (+1) | 0/2 | 148464 (+0) | 5736 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 131 (+128) | 3/4 | 149352 (+13460) | 6408 (+720) | 0/1856 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 34 (+31) | 0/2 | 148464 (+12572) | 6376 (+688) | 0/1552 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1212 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1180 `__volatile_metadata`
- 80 `collapse_publish`
- 64 `collapse_setup`
- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 3 `class collapse::Slot<struct `anonymous namespace'::Actuator> `anonymous namespace'::actuator`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 416 `collapse_publish`
- 352 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 352 `collapse_setup`
- 336 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x83d3e4f3::Logger,struct A0x83d3e4f3::Actuator> > > `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 3 `class collapse::Slot<struct `anonymous namespace'::Actuator> `anonymous namespace'::actuator`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 1180 `__volatile_metadata`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 64 `private: static __cdecl <lambda_9477b7422eb55e9b6ba37ce423cc523e>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 48 `private: static __cdecl <lambda_d4dfe99f47f5b7fad8ddb6c783d4caa9>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Command const & __ptr64)`
- 32 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x1c279bf4::Logger,struct A0x1c279bf4::Actuator> > `anonymous namespace'::bus`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 3 `class collapse::Slot<struct `anonymous namespace'::Actuator> `anonymous namespace'::actuator`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 480 `public: void __cdecl `anonymous namespace'::Sensor::send(unsigned int) __ptr64`
- 416 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::~Subscribe<struct `anonymous namespace'::Sample>(void) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1188 `__volatile_metadata`
- 416 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

## Case: nested_publish

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 48 (+0) | 2/0 | 136124 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 52 (+4) | 2/0 | 136188 (+64) | 5720 (+48) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 69 (+21) | 2/2 | 149372 (+13248) | 6416 (+744) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 69 (+21) | 2/2 | 149372 (+13248) | 6416 (+744) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 52 (+0) | 2/0 | 136188 (+0) | 5720 (+0) | 0/112 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 48 (+0) | 2/0 | 136124 (+0) | 5688 (+16) | 0/112 | - | FAIL: no extra RAM |
| sub0x_dynamic | ok | - | - | - | 81 (+33) | 2/2 | 149624 (+13500) | 6504 (+832) | 0/2288 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (-28) | 0/1 | 148588 (+12464) | 6472 (+800) | 0/1712 | pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 20 (+0) | 2/0 | 135996 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 2 (-18) | 0/0 | 135940 (-56) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 69 (+49) | 2/2 | 149340 (+13344) | 6416 (+728) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 69 (+49) | 2/2 | 149340 (+13344) | 6416 (+728) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 135940 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 20 (+0) | 2/0 | 135996 (+0) | 5688 (+0) | 0/32 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 81 (+61) | 2/2 | 149576 (+13580) | 6504 (+816) | 0/2288 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+0) | 0/1 | 148540 (+12544) | 6472 (+784) | 0/1712 | pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 112 `collapse_publish`
- 112 `public: void __cdecl `anonymous namespace'::Node::publish(struct A0x3a397ef0::Sample const & __ptr64)const __ptr64`
- 64 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 24 `class collapse::Slot<struct `anonymous namespace'::Node> `anonymous namespace'::node`
- 8 `class collapse::Slot<struct `anonymous namespace'::Relay> `anonymous namespace'::relay`
- 8 `$unwind$?publish@Node@?A0x3a397ef0@@QEBAXAEBUSample@2@@Z`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 320 `public: virtual void __cdecl `anonymous namespace'::Relay::receive(struct A0x53195f68::Sample const & __ptr64) __ptr64`
- 288 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `protected: void __cdecl sub0::Publish<struct `anonymous namespace'::Sample>::publish(struct `anonymous namespace'::Sample const & __ptr64)const __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 320 `public: virtual void __cdecl `anonymous namespace'::Relay::receive(struct A0x95df8a56::Sample const & __ptr64) __ptr64`
- 288 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `protected: void __cdecl sub0::Publish<struct `anonymous namespace'::Sample>::publish(struct `anonymous namespace'::Sample const & __ptr64)const __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 112 `public: void __cdecl sub0x::Wiring<struct `anonymous namespace'::Relay,struct A0x3d5b643a::Actuator,struct A0x3d5b643a::Tail>::publish<struct `anonymous namespace'::Sample>(struct `anonymous namespace'::Sample const & __ptr64)const __ptr64`
- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Relay,struct A0x3d5b643a::Actuator,struct A0x3d5b643a::Tail> > `anonymous namespace'::bus`
- 8 `class collapse::Slot<struct `anonymous namespace'::Relay> `anonymous namespace'::relay`
- 8 `$unwind$??$publish@USample@?A0x3d5b643a@@@?$Wiring@URelay@?A0x3d5b643a@@UActuator@2@UTail@2@@sub0x@@QEBAXAEBUSample@?A0x3d5b643a@@@Z`
- 7 `class collapse::Slot<struct `anonymous namespace'::Tail> `anonymous namespace'::tail`
- 1 `class collapse::Slot<struct `anonymous namespace'::Actuator> `anonymous namespace'::actuator`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 112 `public: static void __cdecl sub0x::StaticWiring<&class collapse::Slot<struct `anonymous namespace'::Relay> `anonymous namespace'::relay,&class collapse::Slot<struct `anonymous namespace'::Actuator> A0xaa84cffc::actuator,&class collapse::Slot<struct `anonymous namespace'::Tail> A0xaa84cffc::tail>::publish<struct `anonymous namespace'::Sample>(struct `anonymous namespace'::Sample const & __ptr64)`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 8 `class collapse::Slot<struct `anonymous namespace'::Relay> `anonymous namespace'::relay`
- 8 `$unwind$??$publish@USample@?A0xaa84cffc@@@?$StaticWiring@$MPEAV?$Slot@URelay@?A0xaa84cffc@@@collapse@@1?relay@?A0xaa84cffc@@3V12@A$MPEAV?$Slot@UActuator@?A0xaa84cffc@@@2@1?actuator@4@3V52@A$MPEAV?$Slot@UTail@?A0xaa84cffc@@@2@1?tail@4@3V72@A@sub0x@@SAXAEBUSample@?A0xaa84cffc@@@Z`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 400 `public: virtual void __cdecl `anonymous namespace'::Relay::receive(struct A0xd1ccce5e::Sample const & __ptr64) __ptr64`
- 320 `collapse_setup`
- 288 `protected: void __cdecl sub0x::Publish<struct `anonymous namespace'::Sample>::publish(struct `anonymous namespace'::Sample const & __ptr64,struct sub0x::PublishReport * __ptr64)const __ptr64`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1188 `__volatile_metadata`
- 320 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 160 `public: virtual void __cdecl `anonymous namespace'::Relay::receive(struct A0x3cb54ca3::Sample const & __ptr64) __ptr64`

</details>

## Case: one_receiver

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 6 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+3) | 0/1 | 148384 (+12476) | 5704 (+16) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 7 (+1) | 0/0 | 135924 (+16) | 5688 (+0) | 0/0 | - | reference; PASS |
| sub0pub_spike | ok | - | - | - | 59 (+53) | 1/2 | 147256 (+11348) | 6056 (+368) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+53) | 1/2 | 147256 (+11348) | 6056 (+368) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+45) | 0/2 | 147224 (+11316) | 6056 (+368) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 7 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 6 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148384 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+63) | 1/2 | 147212 (+11304) | 6016 (+328) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+14) | 0/1 | 146756 (+10848) | 5992 (+304) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 148368 (+12476) | 5704 (+16) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 2 (+0) | 0/0 | 135908 (+16) | 5688 (+0) | 0/0 | - | reference; PASS |
| sub0pub_spike | ok | - | - | - | 59 (+57) | 1/2 | 147240 (+11348) | 6056 (+368) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 147240 (+11348) | 6056 (+368) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 147208 (+11316) | 6056 (+368) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148368 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 147196 (+11304) | 6016 (+328) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 146740 (+10848) | 5992 (+304) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1212 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 32 `collapse_setup`
- 8 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `collapse_publish`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 8 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 32 `private: static __cdecl <lambda_c98bae648c8d7504b41febb89c3c5a7b>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`
- 8 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller> > `anonymous namespace'::bus`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 288 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::~Subscribe<struct `anonymous namespace'::Sample>(void) __ptr64`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1188 `__volatile_metadata`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 144 `struct A0x8417fefe::_Removing::_Tables<16,8> const `anonymous namespace'::_Removing::_Tables_8_avx`
- 144 `public: virtual __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::~Subscribe<struct `anonymous namespace'::Sample>(void) __ptr64`

</details>

## Case: publisher_ergonomics

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-4) | 0/1 | 148480 (+12524) | 5736 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 22 (+9) | 0/0 | 136036 (+80) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148480 (+0) | 5736 (+0) | 0/0 | - | PASS |
| alt6_static_bound | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+6) | 0/1 | 148416 (+12508) | 5736 (+48) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 135956 (+48) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148416 (+0) | 5736 (+0) | 0/0 | - | PASS |
| alt6_static_bound | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 135956 (+0) | 5720 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1220 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1196 `__volatile_metadata`
- 96 `collapse_publish`
- 80 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt1_baseline_template (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xab9dd61b::Controller,struct A0xab9dd61b::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt2_crtp_mixin (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x9081060a::Controller,struct A0x9081060a::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt3_ctad_factory (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xe247069e::Controller,struct A0xe247069e::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt4_call_site_out (bytes)</summary>

- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x5d6244e2::Controller,struct A0x5d6244e2::Logger> > `anonymous namespace'::bus`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt5_sink_typeerased (bytes)</summary>

- 1228 `__volatile_metadata`
- 80 `private: static __cdecl <lambda_6b9e63ed2cdab1bce47460dde3fef298>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xfdce5f2f::Controller,struct A0xfdce5f2f::Logger> > `anonymous namespace'::bus`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt6_static_bound (bytes)</summary>

- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt7_deducing_this_mixin (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class `anonymous namespace'::Bus_<struct `anonymous namespace'::Controller,struct A0xf47bfbb9::Controller,struct A0xf47bfbb9::Logger> > > `anonymous namespace'::sensor`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by alt8_deducing_this_callsite (bytes)</summary>

- 1204 `__volatile_metadata`
- 24 `class collapse::Slot<class `anonymous namespace'::Bus_<struct `anonymous namespace'::Controller,struct A0xf88df8ae::Controller,struct A0xf88df8ae::Logger> > `anonymous namespace'::bus`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

## Case: static_dynamic_bridge

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 29 (+0) | 0/1 | 136400 (+0) | 5864 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 27 (-2) | 0/1 | 146968 (+10568) | 5992 (+128) | 0/576 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 20 (-9) | 0/1 | 147340 (+10940) | 6264 (+400) | 0/712 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 29 (+0) | 0/1 | 136400 (+0) | 5896 (+32) | 0/160 | - | FAIL: no extra RAM |
| sub0x_bridge_slots_cpp23 | ok | - | - | - | 29 (+0) | 0/1 | 136400 (+0) | 5896 (+32) | 0/160 | - | FAIL: no extra RAM |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 22 (+0) | 0/1 | 136352 (+0) | 5864 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 20 (-2) | 0/1 | 146920 (+10568) | 5992 (+128) | 0/576 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 20 (-2) | 0/1 | 147276 (+10924) | 6264 (+400) | 0/664 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 22 (+0) | 0/1 | 136352 (+0) | 5896 (+32) | 0/160 | - | FAIL: no extra RAM |
| sub0x_bridge_slots_cpp23 | ok | - | - | - | 22 (+0) | 0/1 | 136352 (+0) | 5896 (+32) | 0/160 | - | FAIL: no extra RAM |

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1196 `__volatile_metadata`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1196 `__volatile_metadata`
- 288 `collapse_setup`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 232 `class sub0x::StaticAdapter<struct sub0x::StaticWiring<&class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller,&class collapse::Slot<struct `anonymous namespace'::Logger> A0xa082f072::logger>,struct `anonymous namespace'::Sample> `RTTI Type Descriptor'`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 1196 `__volatile_metadata`
- 80 `struct sub0x::DynamicPort<struct `anonymous namespace'::Sample,8>::Receiver `RTTI Type Descriptor'`
- 72 `class collapse::Slot<class sub0x::DynamicPort<struct `anonymous namespace'::Sample,8> > `anonymous namespace'::port`
- 48 `struct `anonymous namespace'::Probe `RTTI Type Descriptor'`
- 40 `const `anonymous namespace'::Probe::`RTTI Complete Object Locator'`
- 40 `sub0x::DynamicPort<struct `anonymous namespace'::Sample,8>::Receiver::`RTTI Base Class Descriptor at (0,-1,0,64)'`
- 40 ``anonymous namespace'::Probe::`RTTI Base Class Descriptor at (0,-1,0,64)'`
- 32 `public: virtual void __cdecl `anonymous namespace'::Probe::receive(struct A0x41cd413e::Sample const & __ptr64) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_slots_cpp23 (bytes)</summary>

- 1196 `__volatile_metadata`
- 80 `struct sub0x::DynamicPort<struct `anonymous namespace'::Sample,8>::Receiver `RTTI Type Descriptor'`
- 72 `class collapse::Slot<class sub0x::DynamicPort<struct `anonymous namespace'::Sample,8> > `anonymous namespace'::port`
- 48 `struct `anonymous namespace'::Probe `RTTI Type Descriptor'`
- 40 `const `anonymous namespace'::Probe::`RTTI Complete Object Locator'`
- 40 `sub0x::DynamicPort<struct `anonymous namespace'::Sample,8>::Receiver::`RTTI Base Class Descriptor at (0,-1,0,64)'`
- 40 ``anonymous namespace'::Probe::`RTTI Base Class Descriptor at (0,-1,0,64)'`
- 32 `public: virtual void __cdecl `anonymous namespace'::Probe::receive(struct A0x008e526d::Sample const & __ptr64) __ptr64`

</details>

## Case: static_dynamic_bridge_churn

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 97 (+0) | 0/2 | 136840 (+0) | 5880 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 109 (+12) | 2/2 | 147480 (+10640) | 6040 (+160) | 0/576 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 89 (-8) | 2/2 | 147788 (+10948) | 6296 (+416) | 0/712 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 97 (+0) | 0/2 | 136840 (+0) | 5912 (+32) | 0/160 | - | FAIL: no extra RAM |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 84 (+0) | 0/2 | 136760 (+0) | 5880 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 91 (+7) | 2/2 | 147384 (+10624) | 6040 (+160) | 0/576 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 89 (+5) | 2/2 | 147724 (+10964) | 6296 (+416) | 0/664 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 84 (+0) | 0/2 | 136760 (+0) | 5912 (+32) | 0/160 | - | FAIL: no extra RAM |

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1204 `__volatile_metadata`
- 432 `collapse_publish`
- 304 `collapse_setup`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1204 `__volatile_metadata`
- 416 `collapse_setup`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 232 `class sub0x::StaticAdapter<struct sub0x::StaticWiring<&class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller,&class collapse::Slot<struct `anonymous namespace'::Logger> A0xb90ec9d6::logger>,struct `anonymous namespace'::Sample> `RTTI Type Descriptor'`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 1196 `__volatile_metadata`
- 80 `struct sub0x::DynamicPort<struct `anonymous namespace'::Sample,8>::Receiver `RTTI Type Descriptor'`
- 72 `class collapse::Slot<class sub0x::DynamicPort<struct `anonymous namespace'::Sample,8> > `anonymous namespace'::port`
- 48 `struct `anonymous namespace'::Probe `RTTI Type Descriptor'`
- 40 `const `anonymous namespace'::Probe::`RTTI Complete Object Locator'`
- 40 `sub0x::DynamicPort<struct `anonymous namespace'::Sample,8>::Receiver::`RTTI Base Class Descriptor at (0,-1,0,64)'`
- 40 ``anonymous namespace'::Probe::`RTTI Base Class Descriptor at (0,-1,0,64)'`
- 32 `public: virtual void __cdecl `anonymous namespace'::Probe::receive(struct A0x1dfd9b55::Sample const & __ptr64) __ptr64`

</details>

## Case: static_dynamic_bridge_empty

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 10 (+0) | 0/0 | 135924 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | - | - | - | 29 (+19) | 0/1 | 136048 (+124) | 5752 (+64) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | - | - | - | 27 (-2) | 0/1 | 136164 (+116) | 5768 (+16) | 0/0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | - | - | - | 20 (-9) | 0/1 | 146968 (+10920) | 6184 (+432) | 0/712 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | - | - | - | 29 (+0) | 0/1 | 136048 (+0) | 5752 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 135892 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | - | - | - | 22 (+19) | 0/1 | 136016 (+124) | 5752 (+64) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | - | - | - | 20 (-2) | 0/1 | 136132 (+116) | 5768 (+16) | 0/0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | - | - | - | 20 (-2) | 0/1 | 146920 (+10904) | 6184 (+432) | 0/664 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | - | - | - | 22 (+0) | 0/1 | 136016 (+0) | 5752 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2: largest symbols added by handwritten_registry (bytes)</summary>

- 1204 `__volatile_metadata`
- 112 `collapse_publish`
- 72 `class collapse::Slot<struct `anonymous namespace'::Registry> `anonymous namespace'::registry`
- 48 `collapse_setup`
- 24 `$chain$2$__isa_available_init`
- 12 `$unwind$collapse_publish`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_broker (bytes)</summary>

- 276 `__acrt_fp_strflt_to_string`
- 96 `collapse_teardown`
- 80 `collapse_setup`
- 80 `class collapse::Slot<class sub0x::Domain<struct `anonymous namespace'::Sample> > `anonymous namespace'::domain`
- 44 `$unwind$?write_double_translated_ansi_nolock@@YA?AUwrite_result@?A0x17268360@@HQEBDIAEAV__crt_cached_ptd_host@@@Z`
- 32 `$xdatasym`
- 30 `__acrt_update_thread_locale_data$fin$0`
- 20 `__p__commode`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_inverted (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 232 `class sub0x::StaticAdapter<struct sub0x::StaticWiring<&class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller,&class collapse::Slot<struct `anonymous namespace'::Logger> A0x86ed084e::logger>,struct `anonymous namespace'::Sample> `RTTI Type Descriptor'`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`
- 192 `collapse_setup`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_bridge_slots (bytes)</summary>

- 72 `class collapse::Slot<class sub0x::DynamicPort<struct `anonymous namespace'::Sample,8> > `anonymous namespace'::port`
- 8 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::logger`

</details>

## Case: transport_endpoint

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 135940 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 15 (+2) | 0/1 | 148416 (+12476) | 5720 (+48) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+16) | 5704 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | - | - | - | 13 (+0) | 0/0 | 135956 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 13 (+0) | 0/0 | 135940 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | - | - | - | 13 (+0) | 0/0 | 135940 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 15 (+0) | 0/1 | 148416 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_route | ok | - | - | - | 91 (+78) | 3/2 | 147952 (+12012) | 6128 (+456) | 0/1512 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | - | - | - | 67 (+54) | 0/2 | 147716 (+11776) | 6104 (+432) | 0/1152 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 5 (+0) | 0/0 | 135908 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+4) | 0/1 | 148368 (+12460) | 5720 (+48) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 5 (+0) | 0/0 | 135924 (+16) | 5704 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 5 (+0) | 0/0 | 135924 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | - | - | - | 5 (+0) | 0/0 | 135924 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 5 (+0) | 0/0 | 135908 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | - | - | - | 5 (+0) | 0/0 | 135908 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148368 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_route | ok | - | - | - | 91 (+86) | 3/2 | 147936 (+12028) | 6128 (+456) | 0/1512 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | - | - | - | 67 (+62) | 0/2 | 147700 (+11792) | 6104 (+432) | 0/1152 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1220 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 32 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 16 `class collapse::Slot<struct `anonymous namespace'::Node> `anonymous namespace'::node`
- 7 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radio`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 16 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,class sub0x::Forward<struct `anonymous namespace'::Radio> > > `anonymous namespace'::bus`
- 7 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radio`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire_origin_transport (bytes)</summary>

- 1204 `__volatile_metadata`
- 16 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,class sub0x::Forward<struct `anonymous namespace'::Radio> > > `anonymous namespace'::bus`
- 7 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radio`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 1188 `__volatile_metadata`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static_origin_transport (bytes)</summary>

- 1204 `__volatile_metadata`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 48 `private: static __cdecl <lambda_8fca21f19961b212d652bbd262303ea0>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 16 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,class sub0x::Forward<struct `anonymous namespace'::Radio> > > `anonymous namespace'::bus`
- 7 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radio`
- 1 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_route (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1308 `__volatile_metadata`
- 288 `collapse_setup`
- 288 `public: void __cdecl sub0x::detail::Broker<struct `anonymous namespace'::Sample,struct sub0x::Builtin>::publish(struct `anonymous namespace'::Sample const & __ptr64,void const * __ptr64,struct sub0x::PublishReport * __ptr64)const __ptr64`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 256 `public: void __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::disconnect(void) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_route_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1196 `__volatile_metadata`
- 288 `collapse_setup`
- 272 `collapse_publish`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 240 `public: void __cdecl sub0x::Subscribe<struct `anonymous namespace'::Sample>::disconnect(void) __ptr64`

</details>

## Case: transport_two_links

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 22 (+0) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 27 (+5) | 0/0 | 136052 (+64) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 37 (+10) | 0/0 | 136104 (+52) | 5720 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | - | - | - | 27 (+0) | 0/0 | 136052 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 22 (+0) | 0/0 | 135988 (+0) | 5688 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 15 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 18 (+3) | 0/0 | 136004 (+48) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 26 (+8) | 0/0 | 136036 (+32) | 5720 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | - | - | - | 18 (+0) | 0/0 | 136004 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 15 (+0) | 0/0 | 135956 (+0) | 5688 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1188 `__volatile_metadata`
- 128 `collapse_publish`
- 64 `collapse_setup`
- 24 `class collapse::Slot<struct `anonymous namespace'::Node> `anonymous namespace'::node`
- 8 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 160 `collapse_publish`
- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,class sub0x::Forward<struct `anonymous namespace'::Radio>,class sub0x::Forward<struct `anonymous namespace'::Radio> > > `anonymous namespace'::bus`
- 8 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioB`
- 8 `$unwind$collapse_publish`
- 4 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire_typed_links (bytes)</summary>

- 1196 `__volatile_metadata`
- 24 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xf8983a36::LinkA,struct A0xf8983a36::LinkB> > `anonymous namespace'::bus`
- 8 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controller`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 1188 `__volatile_metadata`
- 4 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Radio> `anonymous namespace'::radioA`

</details>

## Case: two_domains

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 15 (+0) | 0/0 | 135972 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (+7) | 0/2 | 148576 (+12604) | 5752 (+64) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | - | - | - | 22 (+7) | 0/0 | 136036 (+64) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| handwritten_runtime | ok | - | - | - | 23 (+8) | 0/0 | 136052 (+80) | 5720 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | - | - | - | 22 (+0) | 0/0 | 136036 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 23 (+0) | 0/0 | 136052 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | - | - | - | 15 (+0) | 0/0 | 135972 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 15 (+0) | 0/0 | 135972 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/2 | 148576 (+0) | 5752 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_domain | ok | - | - | - | 85 (+70) | 3/2 | 148396 (+12424) | 6224 (+536) | 0/976 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | - | - | - | 35 (+20) | 0/2 | 147612 (+11640) | 6200 (+512) | 0/576 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (+19) | 0/2 | 148512 (+12604) | 5752 (+64) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | - | - | - | 4 (+1) | 0/0 | 135972 (+64) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| handwritten_runtime | ok | - | - | - | 5 (+2) | 0/0 | 135972 (+64) | 5720 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | - | - | - | 4 (+0) | 0/0 | 135972 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 5 (+0) | 0/0 | 135972 (+0) | 5720 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 135908 (+0) | 5688 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/2 | 148512 (+0) | 5752 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_domain | ok | - | - | - | 85 (+82) | 3/2 | 148364 (+12456) | 6224 (+536) | 0/976 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | - | - | - | 35 (+32) | 0/2 | 147580 (+11672) | 6200 (+512) | 0/576 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1212 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_gateway (bytes)</summary>

- 1180 `__volatile_metadata`
- 96 `collapse_publish`
- 80 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 24 `class collapse::Slot<struct `anonymous namespace'::Gateway> `anonymous namespace'::gateway`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by handwritten_runtime (bytes)</summary>

- 1180 `__volatile_metadata`
- 112 `collapse_publish`
- 80 `collapse_setup`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 16 `class collapse::Slot<struct `anonymous namespace'::SensorA> `anonymous namespace'::sensorA`
- 8 `class collapse::Slot<struct `anonymous namespace'::SensorB> `anonymous namespace'::sensorB`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_one_publisher (bytes)</summary>

- 1188 `__volatile_metadata`
- 24 `class collapse::Slot<struct `anonymous namespace'::Gateway<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x6d722769::Logger>,class sub0x::Wiring<struct `anonymous namespace'::Controller> > > `anonymous namespace'::gateway`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0xfa6629e9::Logger> > > `anonymous namespace'::sensorA`
- 8 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct `anonymous namespace'::Controller> > > `anonymous namespace'::sensorB`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_one_publisher (bytes)</summary>

- 1188 `__volatile_metadata`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b2_static (bytes)</summary>

- 1180 `__volatile_metadata`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`
- 4 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 64 `private: static __cdecl <lambda_416e7c5e9574563780f892e4df8c5d4d>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 32 `private: static __cdecl <lambda_03e0deaef35e99dda24c27036ed5e017>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensorB`
- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensorA`
- 16 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller,struct A0x659256ee::Logger> > `anonymous namespace'::busA`
- 8 `class collapse::Slot<struct `anonymous namespace'::Controller> `anonymous namespace'::controllerB`
- 8 `class collapse::Slot<class sub0x::Wiring<struct `anonymous namespace'::Controller> > `anonymous namespace'::busB`
- 4 `class collapse::Slot<struct `anonymous namespace'::Logger> `anonymous namespace'::loggerA`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_domain (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 464 `collapse_setup`
- 304 `public: void __cdecl `anonymous namespace'::Sensor::send(unsigned int) __ptr64`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_domain_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1196 `__volatile_metadata`
- 464 `collapse_setup`
- 276 `__acrt_fp_strflt_to_string`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

## Case: zero_receivers

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 148336 (+12444) | 5704 (+32) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+57) | 1/2 | 136592 (+700) | 5928 (+256) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 136592 (+700) | 5928 (+256) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 136560 (+668) | 5928 (+256) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148336 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 136240 (+348) | 5792 (+120) | 0/80 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 135984 (+92) | 5768 (+96) | 0/72 | - | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 148336 (+12444) | 5704 (+32) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+57) | 1/2 | 136592 (+700) | 5928 (+256) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 136592 (+700) | 5928 (+256) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 136560 (+668) | 5928 (+256) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 135892 (+0) | 5672 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 148336 (+0) | 5704 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 136240 (+348) | 5792 (+120) | 0/80 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 135984 (+92) | 5768 (+96) | 0/72 | - | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained |

<details><summary>msvc-O2: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1220 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_spike (bytes)</summary>

- 1292 `__volatile_metadata`
- 224 `collapse_publish`
- 72 `private: static struct sub0::detail::Broker<struct `anonymous namespace'::Sample>::State sub0::detail::Broker<struct `anonymous namespace'::Sample>::state_`
- 64 `class sub0::Publish<struct `anonymous namespace'::Sample> `RTTI Type Descriptor'`
- 56 ``string'`
- 48 `struct `anonymous namespace'::Sensor `RTTI Type Descriptor'`
- 48 `public: virtual void * __ptr64 __cdecl `anonymous namespace'::Sensor::`scalar deleting destructor'(unsigned int) __ptr64`
- 48 `public: virtual void * __ptr64 __cdecl sub0::Publish<struct `anonymous namespace'::Sample>::`scalar deleting destructor'(unsigned int) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual (bytes)</summary>

- 1292 `__volatile_metadata`
- 224 `collapse_publish`
- 72 `private: static struct sub0::detail::Broker<struct `anonymous namespace'::Sample>::State sub0::detail::Broker<struct `anonymous namespace'::Sample>::state_`
- 64 `class sub0::Publish<struct `anonymous namespace'::Sample> `RTTI Type Descriptor'`
- 56 ``string'`
- 48 `struct `anonymous namespace'::Sensor `RTTI Type Descriptor'`
- 48 `public: virtual void * __ptr64 __cdecl `anonymous namespace'::Sensor::`scalar deleting destructor'(unsigned int) __ptr64`
- 48 `public: virtual void * __ptr64 __cdecl sub0::Publish<struct `anonymous namespace'::Sample>::`scalar deleting destructor'(unsigned int) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 1300 `__volatile_metadata`
- 192 `collapse_publish`
- 72 `private: static struct sub0::detail::Broker<struct `anonymous namespace'::Sample>::State sub0::detail::Broker<struct `anonymous namespace'::Sample>::state_`
- 64 `class sub0::Publish<struct `anonymous namespace'::Sample> `RTTI Type Descriptor'`
- 56 ``string'`
- 48 `struct `anonymous namespace'::Sensor `RTTI Type Descriptor'`
- 48 `public: virtual void * __ptr64 __cdecl `anonymous namespace'::Sensor::`scalar deleting destructor'(unsigned int) __ptr64`
- 48 `public: virtual void * __ptr64 __cdecl sub0::Publish<struct `anonymous namespace'::Sample>::`scalar deleting destructor'(unsigned int) __ptr64`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 16 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 16 `private: static __cdecl <lambda_80e87e54a369925f082bbd1c5d8654e3>::<lambda_invoker_cdecl>(void const * __ptr64,struct `anonymous namespace'::Sample const & __ptr64)`
- 8 `class collapse::Slot<class sub0x::Wiring<> > `anonymous namespace'::bus`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic (bytes)</summary>

- 1292 `__volatile_metadata`
- 288 `collapse_publish`
- 72 `private: static struct sub0x::detail::Table<struct `anonymous namespace'::Sample,struct sub0x::Builtin> sub0x::detail::Broker<struct `anonymous namespace'::Sample,struct sub0x::Builtin>::global_`
- 40 `_tls_used`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 32 `$xdatasym`
- 24 `$unwind$collapse_publish`
- 16 `unsigned __int64 `__local_stdio_printf_options'::`2'::_OptionsStorage`

</details>

<details><summary>msvc-O2: largest symbols added by sub0x_dynamic_lean (bytes)</summary>

- 1188 `__volatile_metadata`
- 80 `collapse_publish`
- 72 `private: static struct sub0x::detail::Table<struct `anonymous namespace'::Sample,struct sub0x::config<struct sub0x::DispatchWith<1>,struct sub0x::ContextWith<2>,struct sub0x::NoFilter> > sub0x::detail::Broker<struct `anonymous namespace'::Sample,struct sub0x::config<struct sub0x::DispatchWith<1>,struct sub0x::ContextWith<2>,struct sub0x::NoFilter> >::global_`
- 32 `struct _onexit_table_t module_local_at_quick_exit_table`
- 24 `$chain$2$__isa_available_init`
- 16 `unsigned __int64 `__local_stdio_printf_options'::`2'::_OptionsStorage`
- 12 `$unwind$collapse_publish`
- 8 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`

</details>


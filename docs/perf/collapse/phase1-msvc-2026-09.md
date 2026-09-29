# Collapse evidence (issue #9)

Final-link evidence per case, build and form; every variant is compared with `handwritten` (equal-work reference, same build and form), or with the extra reference it names, shown as `variant (vs handwritten_<kind>)`: e.g. `handwritten_runtime`, hand-written code that reaches its receivers through addresses stored at setup. Deltas in parentheses. instr = callgrind instructions (publish: per publication of 1000). path = static instructions of `collapse_publish` plus directly reachable functions.

- **msvc-O2**: `Microsoft (R) C/C++ Optimizing Compiler Version 19.51.36246 for x64` `/O2`

## Case: cancellation

### msvc-O2, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 21 (+0) | 0/0 | 131624 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 24 (+3) | 0/0 | 131664 (+40) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_alt1_bool | ok | - | - | - | 22 (+1) | 0/0 | 131624 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | - | - | - | 24 (+0) | 0/0 | 131656 (-8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | - | - | - | 22 (+1) | 0/0 | 131632 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | - | - | - | 22 (+1) | 0/0 | 131624 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | - | - | - | 22 (+1) | 0/0 | 131624 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | - | - | - | 36 (+15) | 0/0 | 131864 (+240) | 11032 (+12) | 0/1 | TLS | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_alt4_filter | ok | - | - | - | 25 (+4) | 0/0 | 131640 (+16) | 11020 (+0) | 0/0 | - | FAIL: publish path |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 9 (+0) | 0/0 | 131560 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 10 (+1) | 0/0 | 131600 (+40) | 11052 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_alt1_bool | ok | - | - | - | 9 (+0) | 0/0 | 131560 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt1_bool_b1 (vs handwritten_runtime) | ok | - | - | - | 10 (+0) | 0/0 | 131592 (-8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_alt1c_expected_cpp23 | ok | - | - | - | 9 (+0) | 0/0 | 131568 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | - | - | - | 9 (+0) | 0/0 | 131560 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt3_static | ok | - | - | - | 9 (+0) | 0/0 | 131560 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt3_tls | ok | - | - | - | 19 (+10) | 0/0 | 131784 (+224) | 11020 (+0) | 0/1 | TLS | FAIL: publish path, no Sub0Pub retained, no extra dependencies |
| sub0x_alt4_filter | ok | - | - | - | 10 (+1) | 0/0 | 131576 (+16) | 11020 (+0) | 0/0 | - | PASS |

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
| handwritten | ok | - | - | - | 26 (+0) | 0/0 | 131648 (+0) | 11020 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | - | - | - | 30 (+4) | 0/0 | 131664 (+16) | 11020 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_alt2_token | ok | - | - | - | 30 (+4) | 0/0 | 131664 (+16) | 11020 (+0) | 0/0 | - | FAIL: publish path |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 11 (+0) | 0/0 | 131584 (+0) | 11020 (+0) | 0/0 | - | reference |
| sub0x_alt1_bool | ok | - | - | - | 11 (+0) | 0/0 | 131584 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_alt2_token | ok | - | - | - | 11 (+0) | 0/0 | 131584 (+0) | 11020 (+0) | 0/0 | - | PASS |

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
| handwritten | ok | - | - | - | 27 (+0) | 3/0 | 131672 (+0) | 11032 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 13 (-14) | 1/1 | 143640 (+11968) | 11620 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 27 (+0) | 3/0 | 131728 (+56) | 11064 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+32) | 1/2 | 143464 (+11792) | 11528 (+496) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+24) | 0/2 | 143440 (+11768) | 11528 (+496) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 27 (+0) | 3/0 | 131728 (+0) | 11064 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 27 (+0) | 3/0 | 131680 (+8) | 11032 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 13 (+0) | 1/1 | 143648 (+8) | 11620 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 17 (+0) | 3/0 | 131640 (+0) | 11032 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 13 (-4) | 1/1 | 143608 (+11968) | 11620 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 17 (+0) | 3/0 | 131696 (+56) | 11064 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+42) | 1/2 | 143432 (+11792) | 11528 (+496) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+34) | 0/2 | 143408 (+11768) | 11528 (+496) | 865/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 17 (+0) | 3/0 | 131696 (+0) | 11064 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 17 (+0) | 3/0 | 131648 (+8) | 11032 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 13 (+0) | 1/1 | 143616 (+8) | 11620 (+0) | 0/0 | - | PASS |

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
| handwritten | ok | - | - | - | 80 (+0) | 0/2 | 132384 (+0) | 11264 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | - | - | - | 113 (+33) | 5/2 | 143584 (+11200) | 11532 (+268) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 104 (+24) | 4/2 | 143552 (+11168) | 11532 (+268) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | - | - | - | 172 (+92) | 6/2 | 143296 (+10912) | 11504 (+240) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 86 (+6) | 2/2 | 142864 (+10480) | 11476 (+212) | 0/664 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 80 (+0) | 0/2 | 132352 (+0) | 11264 (+0) | 0/0 | - | reference |
| sub0pub_virtual | ok | - | - | - | 113 (+33) | 5/2 | 143552 (+11200) | 11532 (+268) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 104 (+24) | 4/2 | 143520 (+11168) | 11532 (+268) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic | ok | - | - | - | 172 (+92) | 6/2 | 143264 (+10912) | 11504 (+240) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 86 (+6) | 2/2 | 142832 (+10480) | 11476 (+212) | 0/664 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 12 (+0) | 0/0 | 131552 (+0) | 11004 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-3) | 0/1 | 143496 (+11944) | 11592 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 12 (+0) | 0/0 | 131576 (+24) | 11036 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+47) | 1/2 | 143376 (+11824) | 11512 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+39) | 0/2 | 143352 (+11800) | 11512 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 14 (+2) | 0/0 | 131584 (+8) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 14 (+2) | 0/0 | 131568 (+16) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 14 (+2) | 0/0 | 131576 (+24) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143504 (+8) | 11592 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+57) | 1/2 | 143248 (+11696) | 11520 (+516) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 32 (+20) | 0/2 | 142760 (+11208) | 11480 (+476) | 0/688 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 131520 (+0) | 11004 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 143464 (+11944) | 11592 (+588) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 2 (+0) | 0/0 | 131544 (+24) | 11036 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 143344 (+11824) | 11512 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 143320 (+11800) | 11512 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 131536 (-8) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 131520 (+0) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+8) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143456 (-8) | 11592 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 143216 (+11696) | 11520 (+516) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 32 (+30) | 0/2 | 142728 (+11208) | 11480 (+476) | 0/688 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 26 (+0) | 0/0 | 131640 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (-4) | 0/1 | 143616 (+11976) | 11592 (+572) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 28 (+2) | 0/0 | 131680 (+40) | 11036 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 72 (+46) | 1/2 | 143480 (+11840) | 11528 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 64 (+38) | 0/2 | 143448 (+11808) | 11528 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 28 (+0) | 0/0 | 131672 (-8) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 26 (+0) | 0/0 | 131640 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/1 | 143608 (-8) | 11592 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 84 (+58) | 1/2 | 143496 (+11856) | 11532 (+512) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 33 (+7) | 0/1 | 142904 (+11264) | 11496 (+476) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131528 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (+19) | 0/1 | 143568 (+12040) | 11592 (+572) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 131568 (+40) | 11036 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 72 (+69) | 1/2 | 143448 (+11920) | 11528 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 64 (+61) | 0/2 | 143416 (+11888) | 11528 (+508) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131560 (-8) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 131528 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/1 | 143560 (-8) | 11592 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 84 (+81) | 1/2 | 143464 (+11936) | 11532 (+512) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 33 (+30) | 0/1 | 142872 (+11344) | 11496 (+476) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 69 (+0) | 0/0 | 132216 (+0) | 11132 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-60) | 0/1 | 145000 (+12784) | 11956 (+824) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_loop | ok | - | - | - | 14 (-55) | 0/0 | 131912 (-304) | 11132 (+0) | 0/0 | - | reference; PASS |
| handwritten_runtime | ok | - | - | - | 197 (+128) | 1/0 | 133088 (+872) | 11400 (+268) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 60 (-9) | 1/2 | 145248 (+13032) | 12380 (+1248) | 968/0 | TLS, pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (-18) | 0/2 | 145200 (+12984) | 12380 (+1248) | 968/0 | TLS, pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 197 (+0) | 1/0 | 133384 (+296) | 11400 (+0) | 0/1104 | - | FAIL: no Sub0Pub retained |
| sub0x_b2_static | ok | - | - | - | 71 (+2) | 1/0 | 132264 (+48) | 11148 (+16) | 0/0 | - | FAIL: no extra RAM |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 144936 (-64) | 11944 (-12) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+0) | 1/2 | 145776 (+13560) | 12400 (+1268) | 0/1000 | TLS, pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (-49) | 0/1 | 145208 (+12992) | 12360 (+1228) | 0/856 | pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 131848 (+0) | 11132 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 144248 (+12400) | 11956 (+824) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_loop | ok | - | - | - | 7 (+5) | 0/0 | 131880 (+32) | 11132 (+0) | 0/0 | - | reference; FAIL: publish path |
| handwritten_runtime | ok | - | - | - | 2 (+0) | 0/0 | 132336 (+488) | 11400 (+268) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 60 (+58) | 1/2 | 145232 (+13384) | 12380 (+1248) | 968/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 145184 (+13336) | 12380 (+1248) | 968/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 132632 (+296) | 11400 (+0) | 0/368 | - | FAIL: no Sub0Pub retained |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 131848 (+0) | 11132 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 144200 (-48) | 11944 (-12) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 145760 (+13912) | 12400 (+1268) | 0/1000 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 145192 (+13344) | 12360 (+1228) | 0/856 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 131592 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-4) | 0/1 | 143584 (+11992) | 11608 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 22 (+9) | 0/0 | 131680 (+88) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+46) | 1/2 | 143480 (+11888) | 11560 (+540) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+46) | 1/2 | 143480 (+11888) | 11560 (+540) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+38) | 0/2 | 143456 (+11864) | 11560 (+540) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131672 (-8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 13 (+0) | 0/0 | 131592 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 13 (+0) | 0/0 | 131600 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143576 (-8) | 11608 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+56) | 1/2 | 143352 (+11760) | 11552 (+532) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+7) | 0/1 | 142792 (+11200) | 11528 (+508) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131544 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+6) | 0/1 | 143520 (+11976) | 11608 (+588) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 131600 (+56) | 11052 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+56) | 1/2 | 143448 (+11904) | 11560 (+540) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+56) | 1/2 | 143448 (+11904) | 11560 (+540) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+48) | 0/2 | 143424 (+11880) | 11560 (+540) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131592 (-8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 131544 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_cxx20 | ok | - | - | - | 3 (+0) | 0/0 | 131552 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143512 (-8) | 11608 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+66) | 1/2 | 143320 (+11776) | 11552 (+532) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+17) | 0/1 | 142760 (+11216) | 11528 (+508) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 15 (+0) | 0/0 | 131568 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 17 (+2) | 0/2 | 143640 (+12072) | 11608 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 19 (+4) | 0/0 | 131640 (+72) | 11036 (+16) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 101 (+86) | 2/4 | 144960 (+13392) | 11880 (+860) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 82 (+67) | 0/4 | 144888 (+13320) | 11880 (+860) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 19 (+0) | 0/0 | 131640 (+0) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 15 (+0) | 0/0 | 131576 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 18 (+1) | 0/2 | 143640 (+0) | 11608 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 131 (+116) | 3/4 | 144952 (+13384) | 11948 (+928) | 0/1856 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 34 (+19) | 0/2 | 144000 (+12432) | 11876 (+856) | 0/1552 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131520 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 17 (+14) | 0/2 | 143560 (+12040) | 11608 (+588) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 131576 (+56) | 11036 (+16) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 101 (+98) | 2/4 | 144896 (+13376) | 11880 (+860) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 82 (+79) | 0/4 | 144824 (+13304) | 11880 (+860) | 1880/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131576 (+0) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 131528 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 18 (+1) | 0/2 | 143560 (+0) | 11608 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 131 (+128) | 3/4 | 144888 (+13368) | 11948 (+928) | 0/1856 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 34 (+31) | 0/2 | 143936 (+12416) | 11876 (+856) | 0/1552 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 48 (+0) | 2/0 | 131736 (+0) | 11028 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 52 (+4) | 2/0 | 131808 (+72) | 11076 (+48) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 69 (+21) | 2/2 | 144960 (+13224) | 11912 (+884) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 69 (+21) | 2/2 | 144960 (+13224) | 11912 (+884) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 52 (+0) | 2/0 | 131800 (-8) | 11076 (+0) | 0/112 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 48 (+0) | 2/0 | 131736 (+0) | 11044 (+16) | 0/0 | - | FAIL: no extra RAM |
| sub0x_dynamic | ok | - | - | - | 81 (+33) | 2/2 | 145144 (+13408) | 12068 (+1040) | 0/2288 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (-28) | 0/1 | 144048 (+12312) | 11984 (+956) | 0/1712 | pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 20 (+0) | 2/0 | 131608 (+0) | 11044 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 2 (-18) | 0/0 | 131584 (-24) | 11052 (+8) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 69 (+49) | 2/2 | 144928 (+13320) | 11912 (+868) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 69 (+49) | 2/2 | 144928 (+13320) | 11912 (+868) | 2040/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 131576 (-8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 20 (+0) | 2/0 | 131608 (+0) | 11044 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 81 (+61) | 2/2 | 145096 (+13488) | 12068 (+1024) | 0/2288 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+0) | 0/1 | 144000 (+12392) | 11984 (+940) | 0/1712 | pure virtual | FAIL: no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 6 (+0) | 0/0 | 131544 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+3) | 0/1 | 143480 (+11936) | 11576 (+556) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 7 (+1) | 0/0 | 131568 (+24) | 11020 (+0) | 0/0 | - | reference; PASS |
| sub0pub_spike | ok | - | - | - | 59 (+53) | 1/2 | 142936 (+11392) | 11452 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+53) | 1/2 | 142944 (+11400) | 11452 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+45) | 0/2 | 142912 (+11368) | 11452 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 7 (+0) | 0/0 | 131560 (-8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 6 (+0) | 0/0 | 131544 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143480 (+0) | 11576 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+63) | 1/2 | 142872 (+11328) | 11432 (+412) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+14) | 0/1 | 142312 (+10768) | 11408 (+388) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 143464 (+11936) | 11576 (+556) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 2 (+0) | 0/0 | 131552 (+24) | 11020 (+0) | 0/0 | - | reference; PASS |
| sub0pub_spike | ok | - | - | - | 59 (+57) | 1/2 | 142920 (+11392) | 11452 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 142928 (+11400) | 11452 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 142896 (+11368) | 11452 (+432) | 776/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 2 (+0) | 0/0 | 131544 (-8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143464 (+0) | 11576 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 142856 (+11328) | 11432 (+412) | 0/808 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 142296 (+10768) | 11408 (+388) | 0/664 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 131600 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-4) | 0/1 | 143584 (+11984) | 11608 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 22 (+9) | 0/0 | 131688 (+88) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131688 (+0) | 11052 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131680 (-8) | 11052 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131680 (-8) | 11052 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131680 (-8) | 11052 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143592 (+8) | 11608 (+0) | 0/0 | - | PASS |
| alt6_static_bound | ok | - | - | - | 13 (+0) | 0/0 | 131600 (+0) | 11020 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131688 (+0) | 11052 (+0) | 0/0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | - | - | - | 22 (+0) | 0/0 | 131696 (+8) | 11052 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131552 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+6) | 0/1 | 143520 (+11968) | 11608 (+588) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 131608 (+56) | 11052 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| alt1_baseline_template (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131608 (+0) | 11052 (+0) | 0/0 | - | PASS |
| alt2_crtp_mixin (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131600 (-8) | 11052 (+0) | 0/0 | - | PASS |
| alt3_ctad_factory (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131600 (-8) | 11052 (+0) | 0/0 | - | PASS |
| alt4_call_site_out (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131600 (-8) | 11052 (+0) | 0/0 | - | PASS |
| alt5_sink_typeerased (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143528 (+8) | 11608 (+0) | 0/0 | - | PASS |
| alt6_static_bound | ok | - | - | - | 3 (+0) | 0/0 | 131552 (+0) | 11020 (+0) | 0/0 | - | PASS |
| alt7_deducing_this_mixin (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131608 (+0) | 11052 (+0) | 0/0 | - | PASS |
| alt8_deducing_this_callsite (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131616 (+8) | 11052 (+0) | 0/0 | - | PASS |

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
| handwritten | ok | - | - | - | 29 (+0) | 0/1 | 132032 (+0) | 11208 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 27 (-2) | 0/1 | 142520 (+10488) | 11420 (+212) | 0/576 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 20 (-9) | 0/1 | 142880 (+10848) | 11704 (+496) | 0/584 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 29 (+0) | 0/1 | 132040 (+8) | 11240 (+32) | 0/160 | - | FAIL: no extra RAM, no Sub0Pub retained |
| sub0x_bridge_slots_cpp23 | ok | - | - | - | 29 (+0) | 0/1 | 132040 (+8) | 11240 (+32) | 0/160 | - | FAIL: no extra RAM, no Sub0Pub retained |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 22 (+0) | 0/1 | 131984 (+0) | 11208 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 20 (-2) | 0/1 | 142472 (+10488) | 11420 (+212) | 0/576 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 20 (-2) | 0/1 | 142816 (+10832) | 11704 (+496) | 0/584 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 22 (+0) | 0/1 | 131992 (+8) | 11240 (+32) | 0/160 | - | FAIL: no extra RAM, no Sub0Pub retained |
| sub0x_bridge_slots_cpp23 | ok | - | - | - | 22 (+0) | 0/1 | 131992 (+8) | 11240 (+32) | 0/160 | - | FAIL: no extra RAM, no Sub0Pub retained |

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
| handwritten | ok | - | - | - | 97 (+0) | 0/2 | 132448 (+0) | 11248 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 109 (+12) | 2/2 | 143016 (+10568) | 11492 (+244) | 0/576 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 89 (-8) | 2/2 | 143312 (+10864) | 11760 (+512) | 0/584 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 97 (+0) | 0/2 | 132456 (+8) | 11280 (+32) | 0/160 | - | FAIL: no extra RAM, no Sub0Pub retained |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 84 (+0) | 0/2 | 132368 (+0) | 11248 (+0) | 0/0 | - | reference |
| sub0x_bridge_broker | ok | - | - | - | 91 (+7) | 2/2 | 142920 (+10552) | 11492 (+244) | 0/576 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_inverted | ok | - | - | - | 89 (+5) | 2/2 | 143248 (+10880) | 11760 (+512) | 0/584 | pure virtual | FAIL: publish path, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots | ok | - | - | - | 84 (+0) | 0/2 | 132376 (+8) | 11280 (+32) | 0/160 | - | FAIL: no extra RAM, no Sub0Pub retained |

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
| handwritten | ok | - | - | - | 10 (+0) | 0/0 | 131568 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | - | - | - | 29 (+19) | 0/1 | 131696 (+128) | 11096 (+76) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | - | - | - | 27 (-2) | 0/1 | 131792 (+96) | 11124 (+28) | 0/0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | - | - | - | 20 (-9) | 0/1 | 142528 (+10832) | 11612 (+516) | 0/584 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | - | - | - | 29 (+0) | 0/1 | 131688 (-8) | 11096 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131536 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_registry | ok | - | - | - | 22 (+19) | 0/1 | 131664 (+128) | 11096 (+76) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0x_bridge_broker (vs handwritten_registry) | ok | - | - | - | 20 (-2) | 0/1 | 131760 (+96) | 11124 (+28) | 0/0 | - | FAIL: no extra RAM |
| sub0x_bridge_inverted (vs handwritten_registry) | ok | - | - | - | 20 (-2) | 0/1 | 142480 (+10816) | 11612 (+516) | 0/584 | pure virtual | FAIL: no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_bridge_slots (vs handwritten_registry) | ok | - | - | - | 22 (+0) | 0/1 | 131656 (-8) | 11096 (+0) | 0/0 | - | PASS |

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
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 131576 (+0) | 11004 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 15 (+2) | 0/1 | 143520 (+11944) | 11592 (+588) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 13 (+0) | 0/0 | 131600 (+24) | 11036 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 13 (+0) | 0/0 | 131600 (+0) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | - | - | - | 13 (+0) | 0/0 | 131616 (+16) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 13 (+0) | 0/0 | 131584 (+8) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | - | - | - | 13 (+0) | 0/0 | 131600 (+24) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 15 (+0) | 0/1 | 143520 (+0) | 11592 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_route | ok | - | - | - | 91 (+78) | 3/2 | 143592 (+12016) | 11580 (+576) | 0/1512 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | - | - | - | 67 (+54) | 0/2 | 143232 (+11656) | 11568 (+564) | 0/1152 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 5 (+0) | 0/0 | 131544 (+0) | 11004 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+4) | 0/1 | 143472 (+11928) | 11592 (+588) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 5 (+0) | 0/0 | 131568 (+24) | 11036 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 5 (+0) | 0/0 | 131568 (+0) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire_origin_transport (vs handwritten_runtime) | ok | - | - | - | 5 (+0) | 0/0 | 131584 (+16) | 11036 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 5 (+0) | 0/0 | 131552 (+8) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b2_static_origin_transport | ok | - | - | - | 5 (+0) | 0/0 | 131568 (+24) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143472 (+0) | 11592 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_route | ok | - | - | - | 91 (+86) | 3/2 | 143576 (+12032) | 11580 (+576) | 0/1512 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_route_lean | ok | - | - | - | 67 (+62) | 0/2 | 143216 (+11672) | 11568 (+564) | 0/1152 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 22 (+0) | 0/0 | 131624 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 27 (+5) | 0/0 | 131696 (+72) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 37 (+10) | 0/0 | 131736 (+40) | 11064 (+12) | 0/0 | - | FAIL: publish path, no extra RAM |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | - | - | - | 27 (+0) | 0/0 | 131704 (+8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 22 (+0) | 0/0 | 131632 (+8) | 11020 (+0) | 0/0 | - | PASS |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 15 (+0) | 0/0 | 131592 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_runtime | ok | - | - | - | 18 (+3) | 0/0 | 131648 (+56) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 26 (+8) | 0/0 | 131680 (+32) | 11052 (+0) | 0/0 | - | FAIL: publish path |
| sub0x_b1_wire_typed_links (vs handwritten_runtime) | ok | - | - | - | 18 (+0) | 0/0 | 131656 (+8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 15 (+0) | 0/0 | 131600 (+8) | 11020 (+0) | 0/0 | - | PASS |

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
| handwritten | ok | - | - | - | 15 (+0) | 0/0 | 131600 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (+7) | 0/2 | 143672 (+12072) | 11624 (+604) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | - | - | - | 22 (+7) | 0/0 | 131672 (+72) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| handwritten_runtime | ok | - | - | - | 23 (+8) | 0/0 | 131688 (+88) | 11052 (+32) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | - | - | - | 22 (+0) | 0/0 | 131680 (+8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 23 (+0) | 0/0 | 131688 (+0) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | - | - | - | 15 (+0) | 0/0 | 131616 (+16) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 15 (+0) | 0/0 | 131608 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/2 | 143672 (+0) | 11624 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_domain | ok | - | - | - | 85 (+70) | 3/2 | 143968 (+12368) | 11736 (+716) | 0/976 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | - | - | - | 35 (+20) | 0/2 | 143152 (+11552) | 11640 (+620) | 0/576 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131536 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 22 (+19) | 0/2 | 143608 (+12072) | 11624 (+604) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_gateway | ok | - | - | - | 4 (+1) | 0/0 | 131608 (+72) | 11052 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| handwritten_runtime | ok | - | - | - | 5 (+2) | 0/0 | 131608 (+72) | 11052 (+32) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0x_b1_one_publisher (vs handwritten_gateway) | ok | - | - | - | 4 (+0) | 0/0 | 131616 (+8) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 5 (+0) | 0/0 | 131608 (+0) | 11052 (+0) | 0/0 | - | PASS |
| sub0x_b2_one_publisher | ok | - | - | - | 3 (+0) | 0/0 | 131552 (+16) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 131544 (+8) | 11020 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 22 (+0) | 0/2 | 143608 (+0) | 11624 (+0) | 0/0 | - | PASS |
| sub0x_dynamic_domain | ok | - | - | - | 85 (+82) | 3/2 | 143936 (+12400) | 11736 (+716) | 0/976 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_domain_lean | ok | - | - | - | 35 (+32) | 0/2 | 143120 (+11584) | 11640 (+620) | 0/576 | pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |

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
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11004 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 143440 (+11912) | 11576 (+572) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+57) | 1/2 | 132336 (+808) | 11264 (+260) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 132336 (+808) | 11264 (+260) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 132312 (+784) | 11264 (+260) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143432 (-8) | 11576 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 132000 (+472) | 11112 (+108) | 0/80 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 131616 (+88) | 11112 (+108) | 0/72 | - | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained |

### msvc-O2, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11004 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+7) | 0/1 | 143440 (+11912) | 11576 (+572) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| sub0pub_spike | ok | - | - | - | 59 (+57) | 1/2 | 132336 (+808) | 11264 (+260) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual | ok | - | - | - | 59 (+57) | 1/2 | 132336 (+808) | 11264 (+260) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+49) | 0/2 | 132312 (+784) | 11264 (+260) | 336/0 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 2 (+0) | 0/0 | 131528 (+0) | 11004 (+0) | 0/0 | - | PASS |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143432 (-8) | 11576 (+0) | 0/0 | - | PASS |
| sub0x_dynamic | ok | - | - | - | 69 (+67) | 1/2 | 132000 (+472) | 11112 (+108) | 0/80 | TLS | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_dynamic_lean | ok | - | - | - | 20 (+18) | 0/1 | 131616 (+88) | 11112 (+108) | 0/72 | - | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained |

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


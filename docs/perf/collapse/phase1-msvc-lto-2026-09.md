# Collapse evidence (issue #9)

Final-link evidence per case, build and form; every variant is compared with `handwritten` (equal-work reference, same build and form), or with the extra reference it names, shown as `variant (vs handwritten_<kind>)`: e.g. `handwritten_runtime`, hand-written code that reaches its receivers through addresses stored at setup. Deltas in parentheses. instr = callgrind instructions (publish: per publication of 1000). path = static instructions of `collapse_publish` plus directly reachable functions.

- **msvc-O2-lto**: `Microsoft (R) C/C++ Optimizing Compiler Version 19.51.36246 for x64` `/O2 /GL`

## Case: cancellation

## Case: cancellation_filtered

## Case: cross_file

### msvc-O2-lto, observable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 13 (+0) | 0/0 | 131560 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (-4) | 0/1 | 143560 (+12000) | 11600 (+580) | 0/0 | - | reference; FAIL: no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 16 (+3) | 0/0 | 131632 (+72) | 11044 (+24) | 0/0 | - | reference; FAIL: publish path, no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+46) | 1/2 | 143464 (+11904) | 11528 (+508) | 753/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+38) | 0/2 | 143424 (+11864) | 11528 (+508) | 753/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 16 (+0) | 0/0 | 131632 (+0) | 11044 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 13 (+0) | 0/0 | 131568 (+8) | 11028 (+8) | 0/0 | - | FAIL: no extra RAM |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143560 (+0) | 11600 (+0) | 0/0 | - | PASS |

### msvc-O2-lto, removable work

| variant | checksum | publish instr | setup instr | teardown instr | path instr | calls (direct/indirect) | text | data+bss | retained sub0/sub0x (B) | added deps | verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| handwritten | ok | - | - | - | 3 (+0) | 0/0 | 131496 (+0) | 11020 (+0) | 0/0 | - | reference |
| handwritten_erased | ok | - | - | - | 9 (+6) | 0/1 | 143512 (+12016) | 11600 (+580) | 0/0 | - | reference; FAIL: publish path, no extra indirect calls, no extra RAM |
| handwritten_runtime | ok | - | - | - | 4 (+1) | 0/0 | 131552 (+56) | 11044 (+24) | 0/0 | - | reference; FAIL: no extra RAM |
| sub0pub_virtual | ok | - | - | - | 59 (+56) | 1/2 | 143432 (+11936) | 11528 (+508) | 753/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0pub_virtual_lean | ok | - | - | - | 51 (+48) | 0/2 | 143392 (+11896) | 11528 (+508) | 753/0 | TLS, pure virtual | FAIL: publish path, no extra indirect calls, no extra RAM, no Sub0Pub retained, no extra dependencies |
| sub0x_b1_wire (vs handwritten_runtime) | ok | - | - | - | 4 (+0) | 0/0 | 131552 (+0) | 11044 (+0) | 0/0 | - | PASS |
| sub0x_b2_static | ok | - | - | - | 3 (+0) | 0/0 | 131504 (+8) | 11028 (+8) | 0/0 | - | FAIL: no extra RAM |
| sub0x_b3_sink (vs handwritten_erased) | ok | - | - | - | 9 (+0) | 0/1 | 143512 (+0) | 11600 (+0) | 0/0 | - | PASS |

<details><summary>msvc-O2-lto: largest symbols added by handwritten_erased (bytes)</summary>

- 1456 `void __cdecl FindHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,unsigned char,int,unsigned __int64 * __ptr64)`
- 1212 `__volatile_metadata`
- 992 `public: static void __cdecl __FrameHandler4::FrameUnwindToState(unsigned __int64 * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int)`
- 880 `void __cdecl FindHandlerForForeignException<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,int,unsigned __int64 * __ptr64)`
- 614 `$$000000`
- 608 `public: static void * __ptr64 __cdecl __FrameHandler4::CxxCallCatchBlock(struct _EXCEPTION_RECORD * __ptr64)`
- 608 `enum _EXCEPTION_DISPOSITION __cdecl __InternalCxxFrameHandler<class __FrameHandler4>(struct EHExceptionRecord * __ptr64,unsigned __int64 * __ptr64,struct _CONTEXT * __ptr64,struct _xDISPATCHER_CONTEXT * __ptr64,struct FH4::FuncInfo4 * __ptr64,int,unsigned __int64 * __ptr64,unsigned char)`
- 496 `public: void __cdecl FH4::TryBlockMap4::setBuffer(class FH4::TryBlockMap4::iterator) __ptr64`

</details>

<details><summary>msvc-O2-lto: largest symbols added by handwritten_runtime (bytes)</summary>

- 1180 `__volatile_metadata`
- 80 `collapse_setup`
- 80 `collapse_publish`
- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor> `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2-lto: largest symbols added by sub0pub_virtual (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1292 `__volatile_metadata`
- 470 `$$000000`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `collapse_publish`

</details>

<details><summary>msvc-O2-lto: largest symbols added by sub0pub_virtual_lean (bytes)</summary>

- 4352 `struct A0x8417fefe::_Removing::_Tables<256,16> const `anonymous namespace'::_Removing::_Tables_2_sse`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_4_avx`
- 2304 `struct A0x8417fefe::_Removing::_Tables<256,8> const `anonymous namespace'::_Removing::_Tables_1_sse`
- 1300 `__volatile_metadata`
- 470 `$$000000`
- 272 `collapse_setup`
- 272 `struct A0x8417fefe::_Removing::_Tables<16,16> const `anonymous namespace'::_Removing::_Tables_4_sse`
- 224 `void const * __ptr64 __cdecl `anonymous namespace'::_Finding::_Find_impl<struct `anonymous namespace'::_Finding::_Find_traits_8,0,unsigned __int64>(void const * __ptr64,void const * __ptr64 const,unsigned __int64)`

</details>

<details><summary>msvc-O2-lto: largest symbols added by sub0x_b1_wire (bytes)</summary>

- 24 `class collapse::Slot<struct `anonymous namespace'::Sensor<class sub0x::Wiring<struct app::Controller,struct app::Controller,struct app::Logger> > > `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2-lto: largest symbols added by sub0x_b2_static (bytes)</summary>

- 1180 `__volatile_metadata`
- 8 `__scrt_ucrt_dll_is_in_use`
- 4 `class collapse::Slot<struct `anonymous namespace'::Sensor<struct sub0x::StaticWiring<&class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA,&class collapse::Slot<struct app::Controller> A0xd0600b1e::controllerB,&class collapse::Slot<struct app::Logger> A0xd0600b1e::logger> > > `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

<details><summary>msvc-O2-lto: largest symbols added by sub0x_b3_sink (bytes)</summary>

- 64 `private: static __cdecl <lambda_5388d9f02801c667922566f057c90ec7>::<lambda_invoker_cdecl>(void const * __ptr64,struct app::Sample const & __ptr64)`
- 24 `class collapse::Slot<class sub0x::Wiring<struct app::Controller,struct app::Controller,struct app::Logger> > `anonymous namespace'::bus`
- 16 `class collapse::Slot<struct app::Sensor> `anonymous namespace'::sensor`
- 4 `class collapse::Slot<struct app::Logger> `anonymous namespace'::logger`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerB`
- 4 `class collapse::Slot<struct app::Controller> `anonymous namespace'::controllerA`

</details>

## Case: dynamic_subscriptions

## Case: filters

## Case: large_payload

## Case: many_receivers

## Case: multi_receivers

## Case: multi_types

## Case: nested_publish

## Case: one_receiver

## Case: publisher_ergonomics

## Case: static_dynamic_bridge

## Case: static_dynamic_bridge_churn

## Case: static_dynamic_bridge_empty

## Case: transport_endpoint

## Case: transport_two_links

## Case: two_domains

## Case: zero_receivers


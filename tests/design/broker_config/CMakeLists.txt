# PROTOTYPE: per-Data broker configuration (docs/design/BROKER_CUSTOMISATION.md)
# Builds and tests the sub0x design prototype without touching sub0pub.hpp.

set(SUB0X_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SUB0X_VENDOR "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor")

function(sub0x_target name)
    target_link_libraries(${name} PRIVATE Sub0Pub::Sub0Pub)
    target_include_directories(${name} PRIVATE "${SUB0X_DIR}" "${SUB0X_VENDOR}")
    # Project-wide default configuration, applied identically to every TU by the build system
    target_compile_definitions(${name} PRIVATE SUB0X_CONFIG_HEADER="project_config.hpp" ${ARGN})
endfunction()

add_executable(Sub0Pub_Sub0xTests
    ../../test_main.cpp
    test_binding.cpp
    test_multi_tu_a.cpp
    test_multi_tu_b.cpp
)
sub0x_target(Sub0Pub_Sub0xTests SUB0X_TEST_COUNT_MISMATCHES SUB0X_CHECK_CONFIG=true)
add_test(NAME Sub0Pub_Sub0xTests COMMAND Sub0Pub_Sub0xTests)

# Deliberate cross-TU configuration mismatch: passes only if the debug registry detects it
add_executable(Sub0Pub_Sub0xMismatch mismatch_a.cpp mismatch_b.cpp)
sub0x_target(Sub0Pub_Sub0xMismatch SUB0X_TEST_COUNT_MISMATCHES SUB0X_CHECK_CONFIG=true)
add_test(NAME Sub0Pub_Sub0xMismatchDetected COMMAND Sub0Pub_Sub0xMismatch)

# Misuse must be rejected at compile time with the intended diagnostic
function(sub0x_compile_fail name regex)
    add_library(${name} OBJECT EXCLUDE_FROM_ALL compile_fail/${name}.cpp)
    sub0x_target(${name})
    add_test(NAME Sub0Pub_Sub0x_${name}
             COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target ${name} --config $<CONFIG>)
    set_tests_properties(Sub0Pub_Sub0x_${name} PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endfunction()

sub0x_compile_fail(cf_two_places          "configured in exactly one place")
sub0x_compile_fail(cf_filter_disabled     "override")
sub0x_compile_fail(cf_cancel_no_context   "cancel\\(\\) needs a publish context")
sub0x_compile_fail(cf_lock_direct         "a Lock requires Snapshot dispatch")
sub0x_compile_fail(cf_domain_global       "requires a Data type configured with sub0x::Scoped")
sub0x_compile_fail(cf_scoped_needs_domain "no matching|no appropriate default constructor|deleted|default constructor")

# Prototype benchmark: sub0x configurations vs docs/PERFORMANCE_BASELINE.md (run via tests/bench/run_baseline.py).
# No project config header, so the Default configuration is Builtin and must match the baseline.
add_executable(Sub0Pub_Sub0xBench bench_sub0x.cpp)
target_link_libraries(Sub0Pub_Sub0xBench PRIVATE Sub0Pub::Sub0Pub)
target_include_directories(Sub0Pub_Sub0xBench PRIVATE "${SUB0X_DIR}" "${SUB0X_VENDOR}" "${CMAKE_CURRENT_SOURCE_DIR}/../../bench")

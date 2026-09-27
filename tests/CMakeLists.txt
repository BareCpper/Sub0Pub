add_executable(Sub0Pub_Tests
    test_main.cpp
    test_pubsub.cpp
    test_cancel.cpp
    test_subscribe_all.cpp
    test_serialization.cpp
    test_fingerprint.cpp
    test_capacity_bounded.cpp
    test_reentrant_check.cpp
    test_thread_safe.cpp
    test_typeidname.cpp
    test_explicit_instantiation.cpp
    test_defaults.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(Sub0Pub_Tests PRIVATE Sub0Pub::Sub0Pub Threads::Threads)

target_include_directories(Sub0Pub_Tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
)

add_test(NAME Sub0Pub_Tests COMMAND Sub0Pub_Tests)

# Performance benchmarks (not run by ctest; see tests/bench/run_baseline.sh)
# The core benchmark is built once per macro policy so policies are compared on identical scenarios.
function(sub0pub_add_bench name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE Sub0Pub::Sub0Pub)
    target_include_directories(${name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
    target_compile_definitions(${name} PRIVATE ${ARGN})
endfunction()

sub0pub_add_bench(Sub0Pub_Bench            bench/bench_core.cpp SUB0PUB_REENTRANT_CHECK=false)
sub0pub_add_bench(Sub0Pub_Bench_Checked    bench/bench_core.cpp SUB0PUB_REENTRANT_CHECK=true)
sub0pub_add_bench(Sub0Pub_Bench_Full       bench/bench_core.cpp SUB0PUB_REENTRANT_SAFE=true SUB0PUB_CANCEL=true SUB0PUB_FILTER=true)
sub0pub_add_bench(Sub0Pub_Bench_ThreadSafe bench/bench_core.cpp SUB0PUB_THREAD_SAFE=true SUB0PUB_FILTER=true)
sub0pub_add_bench(Sub0Pub_Bench_Ipc        bench/bench_ipc.cpp)

# Per-Data configuration and static wiring (public API, sub0pub.hpp Sections 2 to 4)
add_subdirectory(config)
add_subdirectory(wiring)

# Design prototypes (not public API)
add_subdirectory(design/broker_config)
add_subdirectory(design/quiescence)

# Collapse evidence cases (issue #9)
add_subdirectory(collapse)

# v1 vs v2 comparison (MIGRATION.md "Performance evidence")
add_subdirectory(compare)

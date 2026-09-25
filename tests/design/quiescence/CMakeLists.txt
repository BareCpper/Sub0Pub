# SPIKE: teardown/quiescence mechanisms face-off (docs/design/spikes/quiescence.md, issue #5)
# Standalone from tests/design/broker_config/: does not touch the existing sub0x prototype.

set(QX_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(QX_VENDOR "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor")

add_executable(Sub0Pub_QxTests
    ../../test_main.cpp
    test_quiescence.cpp
)
target_link_libraries(Sub0Pub_QxTests PRIVATE Sub0Pub::Sub0Pub)
target_include_directories(Sub0Pub_QxTests PRIVATE "${QX_DIR}" "${QX_VENDOR}")
target_compile_features(Sub0Pub_QxTests PRIVATE cxx_std_17)
add_test(NAME Sub0Pub_QxTests COMMAND Sub0Pub_QxTests)

# Deliberate mutation: skip the wait in every mechanism. Not run by ctest (expected to be unsafe under
# ASan/TSan); built so the spike's mutation-detection claim is reproducible. See quiescence.md.
add_executable(Sub0Pub_QxTests_Mutated EXCLUDE_FROM_ALL
    ../../test_main.cpp
    test_quiescence.cpp
)
target_link_libraries(Sub0Pub_QxTests_Mutated PRIVATE Sub0Pub::Sub0Pub)
target_include_directories(Sub0Pub_QxTests_Mutated PRIVATE "${QX_DIR}" "${QX_VENDOR}")
target_compile_features(Sub0Pub_QxTests_Mutated PRIVATE cxx_std_17)
target_compile_definitions(Sub0Pub_QxTests_Mutated PRIVATE QX_MUTATE_SKIP_WAIT=1)

# Prototype benchmark, same harness/control conditions as tests/design/broker_config/bench_sub0x.cpp
add_executable(Sub0Pub_QxBench bench_qx.cpp)
target_link_libraries(Sub0Pub_QxBench PRIVATE Sub0Pub::Sub0Pub)
target_include_directories(Sub0Pub_QxBench PRIVATE "${QX_DIR}" "${QX_VENDOR}" "${CMAKE_CURRENT_SOURCE_DIR}/../../bench")
target_compile_features(Sub0Pub_QxBench PRIVATE cxx_std_17)

# C++20 outlook: mechanism 2b (refcount + std::atomic wait/notify instead of yield-spin). Built only if
# the toolchain accepts cxx_std_20; kept separate so the C++17 baseline above never depends on C++20.
include(CheckCXXCompilerFlag)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag("-std=c++20" QX_HAS_CXX20_FLAG)
endif()
if (QX_HAS_CXX20_FLAG OR MSVC)
    add_executable(Sub0Pub_QxTests_Cxx20 EXCLUDE_FROM_ALL
        ../../test_main.cpp
        test_quiescence_cxx20.cpp
    )
    target_link_libraries(Sub0Pub_QxTests_Cxx20 PRIVATE Sub0Pub::Sub0Pub)
    target_include_directories(Sub0Pub_QxTests_Cxx20 PRIVATE "${QX_DIR}" "${QX_VENDOR}")
    target_compile_features(Sub0Pub_QxTests_Cxx20 PRIVATE cxx_std_20)
    add_test(NAME Sub0Pub_QxTests_Cxx20 COMMAND Sub0Pub_QxTests_Cxx20)
endif()

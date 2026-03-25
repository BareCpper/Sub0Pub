add_executable(Sub0Pub_Tests
    test_main.cpp
    test_pubsub.cpp
    test_cancel.cpp
    test_subscribe_all.cpp
    test_serialization.cpp
    test_fingerprint.cpp
)

target_link_libraries(Sub0Pub_Tests PRIVATE Sub0Pub::Sub0Pub)

target_include_directories(Sub0Pub_Tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
)

add_test(NAME Sub0Pub_Tests COMMAND Sub0Pub_Tests)

# Performance benchmarks (not run by ctest, run manually)
add_executable(Sub0Pub_Bench bench_pubsub.cpp)
target_link_libraries(Sub0Pub_Bench PRIVATE Sub0Pub::Sub0Pub)
target_include_directories(Sub0Pub_Bench PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
)

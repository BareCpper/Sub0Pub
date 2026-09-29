# An external project using an installed Sub0Pub (driven by ../run_package_test.cmake)
cmake_minimum_required(VERSION 3.21)
project(Sub0PubConsumer LANGUAGES CXX)

find_package(Sub0Pub 2 REQUIRED)

# Warning levels and preprocessor modes are the consumer's choice: the package must not impose any
get_target_property(options Sub0Pub::Sub0Pub INTERFACE_COMPILE_OPTIONS)
if(options)
    message(FATAL_ERROR "Sub0Pub::Sub0Pub imposes compile options on consumers: ${options}")
endif()

enable_testing()
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE Sub0Pub::Sub0Pub)
add_test(NAME consumer COMMAND consumer)

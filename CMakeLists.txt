project( Sub0Pub )
cmake_minimum_required(VERSION 3.2)

add_library( Sub0Pub INTERFACE )
target_include_directories( Sub0Pub INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )

target_sources( Sub0Pub
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/sub0pub.hpp"
)

add_subdirectory(examples)
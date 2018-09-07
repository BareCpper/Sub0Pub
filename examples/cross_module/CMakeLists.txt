cmake_minimum_required(VERSION 3.1)

add_executable( Sub0Pub_cross_module  )
target_link_libraries(Sub0Pub_cross_module PUBLIC Sub0Pub)

target_sources( Sub0Pub_cross_module 
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/publisher.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/testtypes.hpp"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/publisher.cpp"
)
add_subdirectory(basic_pubsub)
add_subdirectory(multi_type)
add_subdirectory(filtering)
add_subdirectory(cancellation)
add_subdirectory(ipc_pipe)
add_subdirectory(layout_check)
add_subdirectory(minimal_sub0pub)

# cross_module disabled: thread_local members in Broker<> cannot be dllexport'd on MSVC.
# Cross-DLL support requires deeper design work (Phase 4).
# add_subdirectory(cross_module)

# Executable v2 recipes: return nonzero on a broken contract, including Release builds.
find_package(Threads REQUIRED)
foreach(recipe static_paths dynamic_lifetime mixed_paths transport_paths)
    add_executable(Sub0Pub_Example_${recipe} v2/${recipe}.cpp)
    target_link_libraries(Sub0Pub_Example_${recipe} PRIVATE Sub0Pub::Sub0Pub Threads::Threads)
    if(SUB0PUB_BUILD_TESTING AND NOT IS_SUBPROJECT)
        add_test(NAME Sub0Pub_Example_${recipe} COMMAND Sub0Pub_Example_${recipe})
    endif()
endforeach()

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

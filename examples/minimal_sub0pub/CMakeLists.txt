# Minimal publish float + subscribe float
add_executable( Sub0Pub_MinimalExample )

target_link_libraries( Sub0Pub_MinimalExample
    PUBLIC
        Sub0Pub
)

target_sources( Sub0Pub_MinimalExample
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"        
)
cmake_minimum_required(VERSION 3.18)

add_library( Sub0Pub_CrossModule_Publisher SHARED )
target_link_libraries(Sub0Pub_CrossModule_Publisher 
    PUBLIC 
        Sub0Pub )

target_sources( Sub0Pub_CrossModule_Publisher 
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/testtypes.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/publisher.hpp" 
    PRIVATE   
        "${CMAKE_CURRENT_LIST_DIR}/publisher.cpp"
)

target_compile_definitions(Sub0Pub_CrossModule_Publisher 
    PUBLIC
        SUB0PUB_STD
)      

##########################

add_executable( Sub0Pub_CrossModule "" )

target_link_libraries(Sub0Pub_CrossModule
    PUBLIC
        Sub0Pub 
        Sub0Pub_CrossModule_Publisher)
        
target_compile_definitions(Sub0Pub_CrossModule 
    PUBLIC
        SUB0PUB_STD
)

target_sources( Sub0Pub_CrossModule
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/testtypes.hpp"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"        
)
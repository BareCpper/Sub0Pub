# @note 3.7.1 as version use in WIndows Git-Bash
# @todo >3.10 needed for include(GoogleTest)?
cmake_minimum_required(VERSION 3.7.1)

# Detect if Sub0Pub is being included inside another project
# - We can disable test generation etc.
if(DEFINED PROJECT_NAME)
  set(IS_SUBPROJECT ON)
endif()

project(Sub0Pub VERSION 0.1.2 LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 98)

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

#include(GNUInstallDirs)

# @todo Should set cmake 3.10
#include(GoogleTest)

#option(SUB0PUB_USE_VALGRIND "Perform SelfTests with Valgrind" OFF)
option(SUB0PUB_BUILD_TESTING "Build unit-tests" ON)
option(SUB0PUB_BUILD_EXAMPLES "Build examples" ON)
#option(SUB0PUB_ENABLE_COVERAGE "Generate coverage for unit-tests" OFF)
#option(SUB0PUB_ENABLE_WERROR "Enable all warnings as errors" ON)
#option(SUB0PUB_INSTALL_DOCS "Install documentation alongside library" ON)

# Define some folders
# @todo Organise targets into folders i.e Tests etc
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SUB0PUB_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(UNITTEST_DIRECTORY ${SUB0PUB_DIRECTORY}/test)
set(BENCHMARK_DIRECTORY ${SUB0PUB_DIRECTORY}/benchmark)
set(INCLUDE_DIRECTORY ${SUB0PUB_DIRECTORY}/include)

if (BUILD_TESTING AND SUB0PUB_BUILD_TESTING AND NOT_SUBPROJECT)
    add_subdirectory(projects)
endif()

if(SUB0PUB_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Sub0Pub as header only target
# + Namespaced alias for linking against core library from client
add_library(Sub0Pub INTERFACE)
add_library(Sub0Pub::Sub0Pub ALIAS Sub0Pub)

target_include_directories(Sub0Pub
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources( Sub0Pub 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/sub0pub/sub0pub.hpp>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/sub0pub/sub0pub.hpp>
)

# Install project if not included by add_subdirecrory(Sub0Pub) from another project
if (NOT IS_SUBPROJECT)

    include(CMakePackageConfigHelpers)
    
    set(SUB0PUB_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Sub0Pub")
    
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/Sub0PubConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Sub0PubConfig.cmake"
        INSTALL_DESTINATION
          "${SUB0PUB_CMAKE_CONFIG_DESTINATION}"
    )

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/Sub0PubConfigVersion.cmake"
        COMPATIBILITY 
            SameMajorVersion
    )

    # Install Sub0Pub target as Sub0Pub::Sub0Pub
    # @todo Test/verify install
    install(
        TARGETS
            Sub0Pub
        EXPORT
            Sub0PubTargets
        DESTINATION 
            ${CMAKE_INSTALL_LIBDIR}
            
            
        #RUNTIME DESTINATION bin ${CMAKE_INSTALL_BINDIR}
        #LIBRARY DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
        #ARCHIVE DESTINATION lib ${CMAKE_INSTALL_LIBDIR} # should add lib
        
        #LIBRARY DESTINATION lib COMPONENT Runtime
        #ARCHIVE DESTINATION lib COMPONENT Development
        #RUNTIME DESTINATION bin COMPONENT Runtime
        #PUBLIC_HEADER DESTINATION include COMPONENT Development
        #BUNDLE DESTINATION bin COMPONENT Runtime
    )
    
    # Install targets
    # @todo Gain understanding for what this is doing - is it generating cmake for the targets?
    install(
        EXPORT
            Sub0PubTargets
        NAMESPACE
            Sub0Pub::
        DESTINATION
            ${SUB0PUB_CMAKE_CONFIG_DESTINATION}
    )
    
    # Install version and config    
    install(
        FILES 
            "${PROJECT_BINARY_DIR}/Sub0PubConfigVersion.cmake"
            "${PROJECT_BINARY_DIR}/Sub0PubConfig.cmake"
        DESTINATION 
            "${SUB0PUB_CMAKE_CONFIG_DESTINATION}"
    )
    
    # Install include sources
    install(
        DIRECTORY
            "include/"
        DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}"
    )
    
endif()
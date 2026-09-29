# Collapse evidence cases (docs/EVIDENCE.md). ctest checks that every variant behaves exactly like its hand-written
# reference, on every CI compiler. Measurement (instructions, final-link image evidence, Cortex-M33, MSVC) is done by
# collapse_evidence.py.

file(GLOB SUB0PUB_COLLAPSE_CASES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/cases/*")
foreach(case_dir ${SUB0PUB_COLLAPSE_CASES})
    if(NOT IS_DIRECTORY "${case_dir}")
        continue()
    endif()
    get_filename_component(case "${case_dir}" NAME)
    # A variant is <variant>.cpp or a directory <variant>/ of translation units
    file(GLOB entries LIST_DIRECTORIES true "${case_dir}/*")
    set(variants)
    foreach(entry ${entries})
        get_filename_component(name "${entry}" NAME_WE)
        if(IS_DIRECTORY "${entry}" OR entry MATCHES "\\.cpp$")
            list(APPEND variants ${name})
        endif()
    endforeach()
    foreach(form_value 0 1)
        if(form_value)
            set(form observable)
        else()
            set(form removable)
        endif()
        foreach(variant ${variants})
            if(IS_DIRECTORY "${case_dir}/${variant}")
                file(GLOB sources "${case_dir}/${variant}/*.cpp")
            else()
                set(sources "${case_dir}/${variant}.cpp")
            endif()
            set(target Collapse_${case}_${form}_${variant})
            add_executable(${target} driver.cpp ${sources})
            target_link_libraries(${target} PRIVATE Sub0Pub::Sub0Pub)
            target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
            target_compile_definitions(${target} PRIVATE COLLAPSE_OBSERVABLE=${form_value})
            if(NOT variant STREQUAL "handwritten")
                add_test(NAME Collapse_${case}_${form}_${variant}
                         COMMAND ${CMAKE_COMMAND} -DREFERENCE=$<TARGET_FILE:Collapse_${case}_${form}_handwritten>
                                 -DVARIANT=$<TARGET_FILE:${target}>
                                 -P "${CMAKE_CURRENT_SOURCE_DIR}/compare_output.cmake")
            endif()
        endforeach()
    endforeach()
endforeach()

# The evidence tool checks itself: its publish-path parser on synthetic disassembly (nested-template callees, tail
# calls, PLT stubs, MSVC map and dumpbin output), and its gates. The full evidence run is not part of ctest (it
# needs valgrind and the Arm toolchain).
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    add_test(NAME Sub0Pub_CollapseEvidenceGates
             COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/unit/test_evidence_gates.py")
    add_test(NAME Sub0Pub_CollapseEvidenceSelfTest
             COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/collapse_evidence.py" --self-test)
endif()

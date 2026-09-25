# Collapse evidence cases (issue #9). ctest checks that every variant behaves exactly like its handwritten
# reference, on every CI compiler. Measurement (instructions, final-link ELF evidence, Cortex-M33) is done by
# collapse_evidence.py; see docs/design/COLLAPSE_EVIDENCE.md.

set(SUB0PUB_COLLAPSE_PROTOTYPE "${CMAKE_CURRENT_SOURCE_DIR}/../design/broker_config") # #8 runtime-registry prototype

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
            target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${SUB0PUB_COLLAPSE_PROTOTYPE}")
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

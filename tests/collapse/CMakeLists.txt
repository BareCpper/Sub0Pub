# Collapse evidence cases (issue #9). ctest checks that every variant behaves exactly like its handwritten
# reference, on every CI compiler. Measurement (instructions, final-link ELF evidence, Cortex-M33) is done by
# collapse_evidence.py; see docs/design/COLLAPSE_EVIDENCE.md.

file(GLOB SUB0PUB_COLLAPSE_CASES LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/cases/*")
foreach(case_dir ${SUB0PUB_COLLAPSE_CASES})
    if(NOT IS_DIRECTORY "${case_dir}")
        continue()
    endif()
    get_filename_component(case "${case_dir}" NAME)
    file(GLOB variants "${case_dir}/*.cpp")
    foreach(form_value 0 1)
        if(form_value)
            set(form observable)
        else()
            set(form removable)
        endif()
        set(reference Collapse_${case}_${form}_handwritten)
        foreach(variant_src ${variants})
            get_filename_component(variant "${variant_src}" NAME_WE)
            set(target Collapse_${case}_${form}_${variant})
            add_executable(${target} driver.cpp "${variant_src}")
            target_link_libraries(${target} PRIVATE Sub0Pub::Sub0Pub)
            target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
            target_compile_definitions(${target} PRIVATE COLLAPSE_OBSERVABLE=${form_value})
        endforeach()
        foreach(variant_src ${variants})
            get_filename_component(variant "${variant_src}" NAME_WE)
            if(NOT variant STREQUAL "handwritten")
                add_test(NAME Collapse_${case}_${form}_${variant}
                         COMMAND ${CMAKE_COMMAND} -DREFERENCE=$<TARGET_FILE:${reference}>
                                 -DVARIANT=$<TARGET_FILE:Collapse_${case}_${form}_${variant}>
                                 -P "${CMAKE_CURRENT_SOURCE_DIR}/compare_output.cmake")
            endif()
        endforeach()
    endforeach()
endforeach()

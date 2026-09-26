# Collapse evidence cases (issue #9). ctest checks that every variant behaves exactly like its handwritten
# reference, on every CI compiler. Measurement (instructions, final-link ELF evidence, Cortex-M33) is done by
# collapse_evidence.py; see docs/design/COLLAPSE_EVIDENCE.md.

set(SUB0PUB_COLLAPSE_PROTOTYPE "${CMAKE_CURRENT_SOURCE_DIR}/../design/broker_config") # #8 runtime-registry prototype

# Feature gate (docs/design/spikes/cxx23_upgrade.md): a variant may need a feature that not every CI toolchain
# has even at the right standard -- C++23 "deducing this" (P0847) arrives in GCC 14 (GCC 13 and
# arm-none-eabi-g++ 13 reject it); std::expected is missing with clang + libstdc++ 13. Such a variant names its
# need on its second line, `// SUB0X_REQUIRES: <feature>`; unsupported ones are skipped (not built, not tested).
# Probes use try_compile's CXX_STANDARD so they are compiler-agnostic (no raw -std= flag; MSVC included).
set(SUB0PUB_COLLAPSE_PROBE_deducing-this [[
struct S { template<class T> void f(this auto&& self, const T& t) noexcept { (void)self; (void)t; } };
int main() { S s; s.f(1); return 0; }
]])
set(SUB0PUB_COLLAPSE_PROBE_expected [[
#include <expected>
#if !defined(__cpp_lib_expected)
#error no std::expected
#endif
int main() { std::expected<void, int> e; return e ? 0 : 1; }
]])
foreach(feature deducing-this expected)
    set(probe_src "${CMAKE_CURRENT_BINARY_DIR}/probe_${feature}.cpp")
    file(WRITE "${probe_src}" "${SUB0PUB_COLLAPSE_PROBE_${feature}}")
    try_compile(SUB0PUB_COLLAPSE_HAVE_${feature} "${CMAKE_CURRENT_BINARY_DIR}/probe_${feature}"
                SOURCES "${probe_src}" CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
    message(STATUS "Collapse: ${feature} ${SUB0PUB_COLLAPSE_HAVE_${feature}}")
endforeach()

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
            # A variant opts into a non-default C++ standard by making its first source line
            # `// SUB0X_STD: c++23` (see tests/collapse/collapse_evidence.py: variant_std()), and may name a
            # required language feature on its second line, `// SUB0X_REQUIRES: <feature>`. All other variants
            # keep the project-wide CMAKE_CXX_STANDARD (c++17) untouched.
            list(GET sources 0 first_source)
            file(STRINGS "${first_source}" header_lines LIMIT_COUNT 2)
            list(GET header_lines 0 first_line)
            set(cxx_standard "")
            if(first_line MATCHES "^// SUB0X_STD: c\\+\\+([0-9]+)")
                set(cxx_standard ${CMAKE_MATCH_1})
            endif()
            set(skip_unsupported FALSE)
            set(needed_feature "")
            list(LENGTH header_lines header_line_count)
            if(header_line_count GREATER 1)
                list(GET header_lines 1 second_line)
                if(second_line MATCHES "^// SUB0X_REQUIRES: (.+)$")
                    set(needed_feature "${CMAKE_MATCH_1}")
                    if(NOT SUB0PUB_COLLAPSE_HAVE_${needed_feature})
                        set(skip_unsupported TRUE)
                    endif()
                endif()
            endif()
            if(skip_unsupported)
                message(STATUS "Collapse: skipping ${case}/${variant} (${form}) -- compiler lacks ${needed_feature}")
                continue()
            endif()
            set(target Collapse_${case}_${form}_${variant})
            add_executable(${target} driver.cpp ${sources})
            target_link_libraries(${target} PRIVATE Sub0Pub::Sub0Pub)
            target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${SUB0PUB_COLLAPSE_PROTOTYPE}")
            target_compile_definitions(${target} PRIVATE COLLAPSE_OBSERVABLE=${form_value})
            if(cxx_standard)
                set_target_properties(${target} PROPERTIES
                    CXX_STANDARD ${cxx_standard}
                    CXX_STANDARD_REQUIRED ON
                    CXX_EXTENSIONS OFF)
            endif()
            if(NOT variant STREQUAL "handwritten")
                add_test(NAME Collapse_${case}_${form}_${variant}
                         COMMAND ${CMAKE_COMMAND} -DREFERENCE=$<TARGET_FILE:Collapse_${case}_${form}_handwritten>
                                 -DVARIANT=$<TARGET_FILE:${target}>
                                 -P "${CMAKE_CURRENT_SOURCE_DIR}/compare_output.cmake")
            endif()
        endforeach()
    endforeach()
endforeach()

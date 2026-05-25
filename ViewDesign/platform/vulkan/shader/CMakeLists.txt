# delete orphaned shader binaries and headers

file(GLOB_RECURSE SHADER_BINARIES *.spv)
file(GLOB_RECURSE SHADER_HEADERS *.h)

foreach(SHADER_BINARY ${SHADER_BINARIES})
    cmake_path(REMOVE_EXTENSION SHADER_BINARY LAST_ONLY OUTPUT_VARIABLE SHADER)
    if(NOT EXISTS ${SHADER})
        file(REMOVE "${SHADER_BINARY}")
    endif()
endforeach()

foreach(SHADER_HEADER ${SHADER_HEADERS})
    cmake_path(REMOVE_EXTENSION SHADER_HEADER LAST_ONLY OUTPUT_VARIABLE SHADER_BINARY)
    if(NOT EXISTS ${SHADER_BINARY})
        file(REMOVE "${SHADER_HEADER}")
    endif()
endforeach()


# compile shader binaries

find_program(GLSLC_EXECUTABLE glslc)

if (NOT GLSLC_EXECUTABLE)
    return()
endif()

file(GLOB_RECURSE SHADERS *.vert *.frag)

set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SHADERS}
)

foreach(SHADER ${SHADERS})
    set(SHADER_BINARY ${SHADER}.spv)
    if(${SHADER} IS_NEWER_THAN ${SHADER_BINARY})
        execute_process(COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER_BINARY})
    endif()
endforeach()


# generate shader headers

file(GLOB_RECURSE SHADER_BINARIES *.spv)

foreach(SHADER_BINARY ${SHADER_BINARIES})
    set(SHADER_HEADER ${SHADER_BINARY}.h)
    if(${SHADER_BINARY} IS_NEWER_THAN ${SHADER_HEADER})
        file(READ ${SHADER_BINARY} SHADER_BINARY_CONTENT HEX)
        string(REGEX MATCHALL ".." BYTES ${SHADER_BINARY_CONTENT})

        set(LINES "")
        set(LINE "")
        set(WORD_COUNT 0)
        set(WORD "")
        set(BYTE_COUNT 0)

        foreach(BYTE IN LISTS BYTES)
            string(PREPEND WORD "${BYTE}")
            math(EXPR BYTE_COUNT "${BYTE_COUNT} + 1")
            if(BYTE_COUNT EQUAL 4)
                string(APPEND LINE "0x${WORD}, ")
                math(EXPR WORD_COUNT "${WORD_COUNT} + 1")
                if(WORD_COUNT EQUAL 8)
                    string(APPEND LINES "\t\t${LINE}\n")
                    set(LINE "")
                    set(WORD_COUNT 0)
                endif()
                set(WORD "")
                set(BYTE_COUNT 0)
            endif()
        endforeach()
        if(BYTE_COUNT)
            message("Invalid shader binary file: ${SHADER_BINARY}.")
        endif()
        if(WORD_COUNT)
            string(APPEND LINES "\t\t${LINE}\n")
        endif()

        cmake_path(GET SHADER_BINARY STEM LAST_ONLY SHADER_BINARY_NAME)
        string(REPLACE "." "_" SHADER_STRUCT_NAME ${SHADER_BINARY_NAME})
        file(WRITE ${SHADER_HEADER}

"\
#pragma once

#include <cstdint>

namespace ViewDesign::Vulkan::Shaders {

struct ${SHADER_STRUCT_NAME} {
\tstatic constexpr uint32_t code[] = {\n${LINES}\t};
};

} // namespace ViewDesign::Vulkan::Shaders
"

        )
    endif()
endforeach()

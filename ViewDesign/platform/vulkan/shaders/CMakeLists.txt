find_program(GLSLC_EXECUTABLE glslc REQUIRED)

file(GLOB_RECURSE SHADERS *.vert *.frag)
foreach(SHADER ${SHADERS})
    execute_process(COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER}.spv)
endforeach()


file(GLOB_RECURSE SHADER_BINARIES *.spv)
foreach(SHADER_BINARY ${SHADER_BINARIES})
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
                string(APPEND LINES "\t${LINE}\n")
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
        string(APPEND LINES "\t${LINE}\n")
    endif()

    get_filename_component(SHADER_NAME ${SHADER_BINARY} NAME)
    string(REPLACE "." "_" SHADER_VAR_NAME ${SHADER_NAME})
    file(WRITE ${SHADER_BINARY}.h
"\
#pragma once

#include <cstdint>

namespace ViewDesign {

namespace Vulkan {

constexpr uint32_t ${SHADER_VAR_NAME}[] = {\n${LINES}};

} // namespace Vulkan

} // namespace ViewDesign
"
    )
endforeach()

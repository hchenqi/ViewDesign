cmake_minimum_required(VERSION 3.20)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_definitions("$<$<PLATFORM_ID:Windows>:UNICODE;_UNICODE>")

project(ViewDesign CXX)

add_subdirectory(ViewDesign)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_subdirectory(Test)
endif()

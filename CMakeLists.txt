cmake_minimum_required(VERSION 3.20)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

project(ViewDesign CXX)

add_library(ViewDesign STATIC)

target_include_directories(ViewDesign PUBLIC ${PROJECT_SOURCE_DIR})

add_subdirectory(ViewDesign)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_subdirectory(Test)
endif()

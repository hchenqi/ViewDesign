cmake_minimum_required(VERSION 3.20)

project(WndDesign CXX)

add_subdirectory(WndDesign)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_subdirectory(Test)
endif()

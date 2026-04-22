if(NOT BACKEND STREQUAL "win32")
    return()
endif()

if(NOT WIN32)
    message(FATAL_ERROR "Backend \"win32\" is only supported on Windows")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

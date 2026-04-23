if(NOT TARGET ViewDesign::Win32)
    message(FATAL_ERROR "Backend \"Win32\" is only supported on Windows.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

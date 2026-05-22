if(NOT TARGET ViewDesign::Platform::Win32)
    message(FATAL_ERROR "Not Win32 platform.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_WIN32)

set(VIEWDESIGN_BACKEND_WIN32 ON CACHE BOOL "")

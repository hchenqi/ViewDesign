if(NOT TARGET ViewDesign::Platform::Win32)
    message(FATAL_ERROR "Not Win32 platform.")
endif()

if(NOT TARGET ViewDesign::Platform::OpenGL)
    message(FATAL_ERROR "OpenGL not found.")
endif()

if(NOT TARGET ViewDesign::Platform::GLAD)
    message(FATAL_ERROR "glad not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_WIN32_OPENGL)

set(VIEWDESIGN_BACKEND_WIN32_OPENGL ON CACHE BOOL "")

if(NOT TARGET ViewDesign::Platform::Stb)
    message("stb not found")
    return()
endif()

if(NOT TARGET ViewDesign::Platform::OpenGL)
    message(FATAL_ERROR "OpenGL not found")
endif()

if(NOT TARGET ViewDesign::Platform::GLAD)
    message(FATAL_ERROR "glad not found")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_STB_OPENGL)

set(VIEWDESIGN_BACKEND_STB_OPENGL ON CACHE BOOL "")

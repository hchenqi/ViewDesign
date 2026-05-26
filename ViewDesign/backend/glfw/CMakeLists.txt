if(NOT TARGET ViewDesign::Platform::GLFW)
    message(FATAL_ERROR "GLFW not found")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_GLFW)

set(VIEWDESIGN_BACKEND_GLFW ON CACHE BOOL "")

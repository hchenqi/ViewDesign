if(NOT TARGET ViewDesign::Platform::Stb)
    message("stb not found")
    return()
endif()

if(NOT TARGET ViewDesign::Platform::Vulkan)
    message(FATAL_ERROR "Vulkan not found")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_STB_VULKAN)

set(VIEWDESIGN_BACKEND_STB_VULKAN ON CACHE BOOL "")

if(NOT TARGET ViewDesign::Platform::Win32)
    message(FATAL_ERROR "not Win32 platform")
endif()

if(NOT TARGET ViewDesign::Platform::Vulkan)
    message(FATAL_ERROR "Vulkan not found")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_WIN32_VULKAN)

set(VIEWDESIGN_BACKEND_WIN32_VULKAN ON CACHE BOOL "")

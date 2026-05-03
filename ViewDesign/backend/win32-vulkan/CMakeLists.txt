if(NOT TARGET ViewDesign::Win32)
    message(FATAL_ERROR "Backend \"Win32-Vulkan\" is only supported on Windows.")
endif()

if(NOT TARGET ViewDesign::Vulkan)
    message(FATAL_ERROR "Vulkan not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_WIN32)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_VULKAN)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_WIN32_VULKAN)

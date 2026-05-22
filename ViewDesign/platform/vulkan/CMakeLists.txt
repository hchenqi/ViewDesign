find_package(Vulkan)

if(NOT Vulkan_FOUND)
    return()
endif()

add_subdirectory(shaders)

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformVulkan STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::Vulkan ALIAS ViewDesignPlatformVulkan)
set_target_properties(ViewDesignPlatformVulkan PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformVulkan PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformVulkan PUBLIC Vulkan::Vulkan)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::Vulkan)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_VULKAN)

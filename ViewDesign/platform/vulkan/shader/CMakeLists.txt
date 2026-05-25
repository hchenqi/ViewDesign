include(VulkanCompileShaders.cmake)

vulkan_compile_shaders(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    NAMESPACE ViewDesign::Vulkan::Shaders
)

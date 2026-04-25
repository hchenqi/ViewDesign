if(NOT TARGET ViewDesign::GLFW)
    message(FATAL_ERROR "GLFW not found.")
endif()

if(NOT TARGET ViewDesign::Vulkan)
    message(FATAL_ERROR "Vulkan not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

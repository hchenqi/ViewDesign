find_package(Vulkan OPTIONAL)

if(NOT Vulkan_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignVulkan STATIC ${LIB_SOURCES})
add_library(ViewDesign::Vulkan ALIAS ViewDesignVulkan)
set_target_properties(ViewDesignVulkan PROPERTIES FOLDER "Platform")

target_include_directories(ViewDesignVulkan PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignVulkan PUBLIC Vulkan::Vulkan)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Vulkan)

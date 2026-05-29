find_package(glfw3 CONFIG QUIET)

if(NOT glfw3_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformGLFW STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::GLFW ALIAS ViewDesignPlatformGLFW)
set_target_properties(ViewDesignPlatformGLFW PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformGLFW PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformGLFW PUBLIC glfw)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::GLFW)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_GLFW)

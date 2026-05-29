find_package(glad CONFIG QUIET)

if(NOT glad_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformGLAD STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::GLAD ALIAS ViewDesignPlatformGLAD)
set_target_properties(ViewDesignPlatformGLAD PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformGLAD PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformGLAD PUBLIC glad::glad)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::GLAD)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_GLAD)

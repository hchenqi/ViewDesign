find_package(OpenGL QUIET)

if(NOT OPENGL_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformOpenGL STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::OpenGL ALIAS ViewDesignPlatformOpenGL)
set_target_properties(ViewDesignPlatformOpenGL PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformOpenGL PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformOpenGL PUBLIC OpenGL::GL)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::OpenGL)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_OPENGL)

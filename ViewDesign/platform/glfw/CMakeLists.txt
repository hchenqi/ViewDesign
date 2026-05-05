find_package(glfw3 CONFIG)

if(NOT glfw3_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignGLFW STATIC ${LIB_SOURCES})
add_library(ViewDesign::GLFW ALIAS ViewDesignGLFW)
set_target_properties(ViewDesignGLFW PROPERTIES FOLDER "Platform")

target_include_directories(ViewDesignGLFW PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignGLFW PUBLIC glfw)

target_link_libraries(ViewDesign PUBLIC ViewDesign::GLFW)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_GLFW)

find_package(glfw3 CONFIG OPTIONAL)

if(NOT glfw3_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignGLFW STATIC ${LIB_SOURCES})
add_library(ViewDesign::GLFW ALIAS ViewDesignGLFW)

target_include_directories(ViewDesignGLFW PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignGLFW PRIVATE glfw)

target_link_libraries(ViewDesign PRIVATE ViewDesign::GLFW)

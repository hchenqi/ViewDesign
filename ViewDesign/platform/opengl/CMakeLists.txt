find_package(OpenGL OPTIONAL)

if(NOT OPENGL_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignOpenGL STATIC ${LIB_SOURCES})
add_library(ViewDesign::OpenGL ALIAS ViewDesignOpenGL)
set_target_properties(ViewDesignOpenGL PROPERTIES FOLDER "Platform")

target_include_directories(ViewDesignOpenGL PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignOpenGL PUBLIC OpenGL::GL)

target_link_libraries(ViewDesign PUBLIC ViewDesign::OpenGL)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_OPENGL)

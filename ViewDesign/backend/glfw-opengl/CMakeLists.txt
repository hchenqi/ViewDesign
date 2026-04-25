if(NOT TARGET ViewDesign::GLFW)
    message(FATAL_ERROR "GLFW not found.")
endif()

if(NOT TARGET ViewDesign::OpenGL)
    message(FATAL_ERROR "OpenGL not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

add_executable(GLFWOpenGLTest main.cpp)
target_link_libraries(GLFWOpenGLTest PRIVATE ViewDesign)

if(NOT TARGET ViewDesign::GLFW)
    message(FATAL_ERROR "GLFW not found.")
endif()

if(NOT TARGET ViewDesign::OpenGL)
    message(FATAL_ERROR "OpenGL not found.")
endif()

if(NOT TARGET ViewDesign::GLAD)
    message(FATAL_ERROR "glad not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_GLFW)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_OPENGL)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_GLFW_OPENGL)

add_executable(GLFWOpenGLTest test.cpp)
target_link_libraries(GLFWOpenGLTest PRIVATE ViewDesign)

if(NOT TARGET ViewDesign::GLFW)
    message(FATAL_ERROR "GLFW library not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

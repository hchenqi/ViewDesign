find_package(glad CONFIG OPTIONAL)

if(NOT glad_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignGLAD STATIC ${LIB_SOURCES})
add_library(ViewDesign::GLAD ALIAS ViewDesignGLAD)
set_target_properties(ViewDesignGLAD PROPERTIES FOLDER "Platform")

target_include_directories(ViewDesignGLAD PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignGLAD PUBLIC glad::glad)

target_link_libraries(ViewDesign PUBLIC ViewDesign::GLAD)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_GLAD)

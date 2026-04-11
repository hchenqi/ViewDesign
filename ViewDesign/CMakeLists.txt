file(GLOB_RECURSE LIB_SOURCES
    *.h
    *.cpp
)

add_library(ViewDesign STATIC ${LIB_SOURCES})
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(ViewDesign PUBLIC ${PARENT_DIR})

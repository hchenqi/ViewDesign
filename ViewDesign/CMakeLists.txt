file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "/platform/")
list(FILTER LIB_SOURCES EXCLUDE REGEX "/backend/")
add_library(ViewDesign STATIC ${LIB_SOURCES})
set_target_properties(ViewDesign PROPERTIES FOLDER "ViewDesign")

target_include_directories(ViewDesign PUBLIC ${PROJECT_SOURCE_DIR})

add_subdirectory(platform)
add_subdirectory(backend)

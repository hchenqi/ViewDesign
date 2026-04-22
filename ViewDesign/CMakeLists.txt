file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "/platform/")
list(FILTER LIB_SOURCES EXCLUDE REGEX "/backend/")

target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

add_subdirectory(platform)
add_subdirectory(backend)

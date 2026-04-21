file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "/backend/")

target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

add_subdirectory(backend)

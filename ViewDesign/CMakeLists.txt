file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "/platform/")
list(FILTER LIB_SOURCES EXCLUDE REGEX "/backend/")

target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

add_subdirectory(platform)
add_subdirectory(backend)

find_package(ICU OPTIONAL COMPONENTS uc)
if (ICU_FOUND)
    target_link_libraries(ViewDesign PRIVATE ICU::uc)
endif()

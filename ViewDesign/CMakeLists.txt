file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "/platform/")
list(FILTER LIB_SOURCES EXCLUDE REGEX "/backend/")

target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

add_subdirectory(platform)
add_subdirectory(backend)

include(CheckIncludeFileCXX)
check_include_file_cxx("icu.h" HAVE_ICU_H)
check_include_file_cxx("unicode/ubrk.h" HAVE_UNICODE_UBRK_H)
if(HAVE_ICU_H OR HAVE_UNICODE_UBRK_H)
    target_link_libraries(ViewDesign PRIVATE icuuc)
endif()

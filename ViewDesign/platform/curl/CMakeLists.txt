find_package(CURL QUIET)

if (NOT CURL_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformCurl STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::Curl ALIAS ViewDesignPlatformCurl)
set_target_properties(ViewDesignPlatformCurl PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformCurl PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformCurl PUBLIC CURL::libcurl)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::Curl)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_CURL)

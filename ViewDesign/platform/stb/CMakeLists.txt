find_package(Stb QUIET)

if(NOT Stb_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformStb STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::Stb ALIAS ViewDesignPlatformStb)
set_target_properties(ViewDesignPlatformStb PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformStb PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(ViewDesignPlatformStb PUBLIC ${Stb_INCLUDE_DIR})

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::Stb)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_STB)

find_package(ICU COMPONENTS uc)

if (NOT ICU_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformICU STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::ICU ALIAS ViewDesignPlatformICU)
set_target_properties(ViewDesignPlatformICU PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformICU PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformICU PUBLIC ICU::uc)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::ICU)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_ICU)

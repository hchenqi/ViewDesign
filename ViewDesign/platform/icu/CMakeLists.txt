find_package(ICU COMPONENTS uc)

if (NOT ICU_FOUND)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignICU STATIC ${LIB_SOURCES})
add_library(ViewDesign::ICU ALIAS ViewDesignICU)
set_target_properties(ViewDesignICU PROPERTIES FOLDER "Platform")

target_include_directories(ViewDesignICU PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignICU PUBLIC ICU::uc)

target_link_libraries(ViewDesign PUBLIC ViewDesign::ICU)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_ICU)

if(NOT WIN32)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformWin32 STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::Win32 ALIAS ViewDesignPlatformWin32)
set_target_properties(ViewDesignPlatformWin32 PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformWin32 PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformWin32 PRIVATE imm32)
target_link_libraries(ViewDesignPlatformWin32 PRIVATE icu)
target_link_libraries(ViewDesignPlatformWin32 PRIVATE dwmapi)
target_link_libraries(ViewDesignPlatformWin32 PRIVATE comctl32)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::Win32)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_WIN32)

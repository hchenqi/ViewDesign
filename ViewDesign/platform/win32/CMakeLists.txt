if(NOT WIN32)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignWin32 STATIC ${LIB_SOURCES})
add_library(ViewDesign::Win32 ALIAS ViewDesignWin32)
set_target_properties(ViewDesignWin32 PROPERTIES FOLDER "Platform")

target_include_directories(ViewDesignWin32 PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignWin32 PRIVATE d3d11)
target_link_libraries(ViewDesignWin32 PRIVATE dcomp)
target_link_libraries(ViewDesignWin32 PRIVATE d2d1)
target_link_libraries(ViewDesignWin32 PRIVATE dwrite)
target_link_libraries(ViewDesignWin32 PRIVATE windowscodecs)
target_link_libraries(ViewDesignWin32 PRIVATE imm32)
target_link_libraries(ViewDesignWin32 PRIVATE icu)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Win32)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_WIN32)

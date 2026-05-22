if(NOT WIN32)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignPlatformDirectX STATIC ${LIB_SOURCES})
add_library(ViewDesign::Platform::DirectX ALIAS ViewDesignPlatformDirectX)
set_target_properties(ViewDesignPlatformDirectX PROPERTIES FOLDER "ViewDesign/Platform")

target_include_directories(ViewDesignPlatformDirectX PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignPlatformDirectX PRIVATE d3d11)
target_link_libraries(ViewDesignPlatformDirectX PRIVATE dcomp)
target_link_libraries(ViewDesignPlatformDirectX PRIVATE d2d1)
target_link_libraries(ViewDesignPlatformDirectX PRIVATE dwrite)
target_link_libraries(ViewDesignPlatformDirectX PRIVATE windowscodecs)

target_link_libraries(ViewDesign PUBLIC ViewDesign::Platform::DirectX)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_DIRECTX)

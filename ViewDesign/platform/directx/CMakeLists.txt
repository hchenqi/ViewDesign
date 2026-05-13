if(NOT WIN32)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
add_library(ViewDesignDirectX STATIC ${LIB_SOURCES})
add_library(ViewDesign::DirectX ALIAS ViewDesignDirectX)
set_target_properties(ViewDesignDirectX PROPERTIES FOLDER "Platform")

target_compile_definitions(ViewDesignDirectX PUBLIC NOMINMAX)

target_include_directories(ViewDesignDirectX PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(ViewDesignDirectX PRIVATE d3d11)
target_link_libraries(ViewDesignDirectX PRIVATE dcomp)
target_link_libraries(ViewDesignDirectX PRIVATE d2d1)
target_link_libraries(ViewDesignDirectX PRIVATE dwrite)
target_link_libraries(ViewDesignDirectX PRIVATE windowscodecs)

target_link_libraries(ViewDesign PUBLIC ViewDesign::DirectX)
target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_PLATFORM_DIRECTX)

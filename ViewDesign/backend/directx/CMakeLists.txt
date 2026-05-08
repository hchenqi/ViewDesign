if(NOT TARGET ViewDesign::DirectX)
    message(FATAL_ERROR "DirectX not found.")
endif()

file(GLOB_RECURSE LIB_SOURCES *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_compile_definitions(ViewDesign PUBLIC VIEWDESIGN_BACKEND_DIRECTX)

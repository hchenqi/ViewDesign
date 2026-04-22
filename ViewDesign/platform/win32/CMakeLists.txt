if(NOT WIN32)
    return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})

target_link_libraries(ViewDesign PRIVATE d3d11)
target_link_libraries(ViewDesign PRIVATE dcomp)
target_link_libraries(ViewDesign PRIVATE d2d1)
target_link_libraries(ViewDesign PRIVATE dwrite)
target_link_libraries(ViewDesign PRIVATE windowscodecs)
target_link_libraries(ViewDesign PRIVATE imm32)
target_link_libraries(ViewDesign PRIVATE icuuc)

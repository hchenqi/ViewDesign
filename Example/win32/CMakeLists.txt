if(NOT DEFINED VIEWDESIGN_BACKEND_WIN32)
    return()
endif()

file(GLOB EXAMPLE_SOURCES *.cpp)

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} WIN32 ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE ViewDesign)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "Example/Win32")
endforeach()

if (MSVC)
    target_compile_options(HotReload PRIVATE /ZI)
    target_link_options(HotReload PRIVATE /INCREMENTAL)
endif()

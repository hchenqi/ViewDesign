if(NOT DEFINED VIEWDESIGN_BACKEND_WIN32_VULKAN)
    return()
endif()

add_subdirectory(shader)

file(GLOB EXAMPLE_SOURCES *.cpp)

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} WIN32 ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE ViewDesign)
    set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "Example/Win32-Vulkan")
endforeach()

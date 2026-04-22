file(GLOB TEST_SOURCES
    *.cpp
)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} WIN32 ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE ViewDesign)
    if(MINGW)
        target_link_options(${TEST_NAME} PRIVATE -Wl,--whole-archive $<TARGET_LINKER_FILE:ViewDesign> -Wl,--no-whole-archive)
    endif()
endforeach()

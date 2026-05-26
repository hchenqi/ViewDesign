if(DEFINED VIEWDESIGN_BACKEND)
    set(BACKEND "${VIEWDESIGN_BACKEND}")
endif()

if(NOT DEFINED BACKEND)
    message(FATAL_ERROR "backend not specified")

elseif(BACKEND STREQUAL "Win32-DirectX")
    add_subdirectory(win32)
    add_subdirectory(directx)
    add_subdirectory(win32-directx)

elseif(BACKEND STREQUAL "Win32-OpenGL")
    add_subdirectory(win32)
    add_subdirectory(opengl)
    add_subdirectory(win32-opengl)
    add_subdirectory(text_layout_placeholder)

elseif(BACKEND STREQUAL "Win32-Vulkan")
    add_subdirectory(win32)
    add_subdirectory(vulkan)
    add_subdirectory(win32-vulkan)
    add_subdirectory(text_layout_placeholder)

elseif(BACKEND STREQUAL "GLFW-OpenGL")
    add_subdirectory(glfw)
    add_subdirectory(opengl)
    add_subdirectory(glfw-opengl)
    add_subdirectory(text_layout_placeholder)

elseif(BACKEND STREQUAL "GLFW-Vulkan")
    add_subdirectory(glfw)
    add_subdirectory(vulkan)
    add_subdirectory(glfw-vulkan)
    add_subdirectory(text_layout_placeholder)

else()
    message(FATAL_ERROR "backend: ${BACKEND} not supported")
endif()

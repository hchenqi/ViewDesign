#pragma once

#include <GLFW/glfw3.h>


namespace ViewDesign {

using Handle = void*;


namespace GLFW {


inline GLFWwindow* AsGLFWWindow(Handle window) {
	return static_cast<GLFWwindow*>(window);
}


} // namespace GLFW

} // namespace ViewDesign

#pragma once

#include <GLFW/glfw3.h>


namespace ViewDesign {

using Handle = void*;


namespace GLFW {


inline GLFWwindow* AsGLFWWindow(Handle handle) {
	return static_cast<GLFWwindow*>(handle);
}


} // namespace GLFW

} // namespace ViewDesign

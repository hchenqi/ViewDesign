#pragma once

#include "ViewDesign/common/type.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {


namespace GLFW {


inline GLFWwindow* AsGLFWWindow(Handle window) {
	return static_cast<GLFWwindow*>(window);
}


} // namespace GLFW

} // namespace ViewDesign

#include "ViewDesign/system/desktop.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {


Size GetDesktopSize() {
	int x, y, width, height;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &x, &y, &width, &height);
	return Size(width, height);
}


} // namespace ViewDesign

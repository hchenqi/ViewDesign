#include "ViewDesign/system/desktop.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {


SizeU GetDesktopSize() {
	int x, y, width, height;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &x, &y, &width, &height);
	return SizeU(width, height);
}


} // namespace ViewDesign

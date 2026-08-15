#include "Popup.h"

#if defined(VIEWDESIGN_BACKEND_WIN32)
#include <ViewDesign/platform/win32/window.h>
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW)
#include <ViewDesign/platform/glfw/window.h>
#endif


namespace ViewDesign {


Popup::Popup(Point point, child_type child) : Window(u"", std::move(child)), point(point) {
#if defined(VIEWDESIGN_BACKEND_WIN32)
		Win32::SetWndUndecorated(Win32::AsHWND(GetHandle()));
		Win32::SetWndExStyle(Win32::AsHWND(GetHandle()), WS_EX_TOOLWINDOW);
		Win32::SetWndTopMost(Win32::AsHWND(GetHandle()));
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW)
		glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_FLOATING, GLFW_TRUE);
#endif
}


} // namespace ViewDesign

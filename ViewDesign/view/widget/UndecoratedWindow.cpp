#include "ViewDesign/view/widget/UndecoratedWindow.h"

#if defined(VIEWDESIGN_BACKEND_WIN32)
#include "ViewDesign/platform/win32/window.h"
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW)
#include "ViewDesign/platform/glfw/window.h"
#endif


namespace ViewDesign {


UndecoratedWindow::UndecoratedWindow(const u16string& title, const Style& style, child_type child) : DefaultWindow(title, style, std::move(child)) {
#if defined(VIEWDESIGN_BACKEND_WIN32)
	Win32::SetWndUndecorated(Win32::AsHWND(GetHandle()));
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW)
	glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_DECORATED, GLFW_FALSE);
#endif
}


} // namespace ViewDesign

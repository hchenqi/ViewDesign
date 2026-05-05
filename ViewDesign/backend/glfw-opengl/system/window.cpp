#include "ViewDesign/system/window.h"
#include "ViewDesign/platform/glfw/string.h"
#include "ViewDesign/platform/glfw/window.h"
#include "ViewDesign/platform/glfw/key.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct WindowApi : Window {
	using Window::HasParent;
	using Window::SetSize;
	using Window::SetPoint;
	using Window::SetScale;
	using Window::State;
	using Window::SetState;
	using Window::Redraw;
};

struct DesktopApi : Desktop {
	using Desktop::LoseTrack;
	using Desktop::LoseCapture;
	using Desktop::LoseFocus;
	using Desktop::DispatchMouseEvent;
	using Desktop::DispatchKeyEvent;
};

using namespace GLFW;


namespace {

inline ref_ptr<WindowApi> GetWindow(GLFWwindow* glfw_window) {
	return static_cast<ref_ptr<WindowApi>>(glfwGetWindowUserPointer(glfw_window));
}


void WindowCloseCallback(GLFWwindow* glfw_window) {}


void WindowSizeCallback(GLFWwindow* glfw_window, int width, int height) {
	//
}

void FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height) {
	GetWindow(glfw_window)->SetSize(Size(width, height));
}

void WindowPosCallback(GLFWwindow* glfw_window, int xpos, int ypos) {
	GetWindow(glfw_window)->SetPoint(Point(xpos, ypos));
}

void WindowContentScaleCallback(GLFWwindow* glfw_window, float xscale, float yscale) {
	GetWindow(glfw_window)->SetScale(Scale(xscale, yscale));
}


void WindowIconifyCallback(GLFWwindow* glfw_window, int iconified) {
	GetWindow(glfw_window)->SetState(iconified ? Window::State::Minimized : Window::State::Normal);
}

void WindowMaximizeCallback(GLFWwindow* glfw_window, int maximized) {
	GetWindow(glfw_window)->SetState(maximized ? Window::State::Maximized : Window::State::Normal);
}

void WindowRefreshCallback(GLFWwindow* glfw_window) {
	GetWindow(glfw_window)->Redraw(region_infinite);
}


Point cursor_position;

void CursorEnterCallback(GLFWwindow* glfw_window, int entered) {
	if (entered == GLFW_FALSE) {
		static_cast<DesktopApi&>(desktop).LoseTrack();
	}
}

void CursorPosCallback(GLFWwindow* glfw_window, double xpos, double ypos) {
	MouseEvent mouse_event;
	mouse_event.point = cursor_position = Point(xpos, ypos);
	mouse_event.type = MouseEvent::Move;
	static_cast<DesktopApi&>(desktop).DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
}

void MouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods) {
	MouseEvent mouse_event;
	mouse_event.point = cursor_position;
	switch (action) {
	case GLFW_PRESS:
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: mouse_event.type = MouseEvent::LeftDown; break;
		case GLFW_MOUSE_BUTTON_RIGHT: mouse_event.type = MouseEvent::RightDown; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: mouse_event.type = MouseEvent::MiddleDown; break;
		default: return;
		}
		break;
	case GLFW_RELEASE:
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: mouse_event.type = MouseEvent::LeftUp; break;
		case GLFW_MOUSE_BUTTON_RIGHT: mouse_event.type = MouseEvent::RightUp; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: mouse_event.type = MouseEvent::MiddleUp; break;
		default: return;
		}
		break;
	default: return;
	}
	static_cast<DesktopApi&>(desktop).DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
}

void ScrollCallback(GLFWwindow* glfw_window, double xoffset, double yoffset) {
	MouseEvent mouse_event;
	mouse_event.point = cursor_position;
	if (yoffset != 0.0) {
		mouse_event.wheel_delta = yoffset * 120;
		mouse_event.type = MouseEvent::WheelVertical;
		static_cast<DesktopApi&>(desktop).DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
	}
	if (xoffset != 0.0) {
		mouse_event.wheel_delta = xoffset * 120;
		mouse_event.type = MouseEvent::WheelHorizontal;
		static_cast<DesktopApi&>(desktop).DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
	}
}


void WindowFocusCallback(GLFWwindow* glfw_window, int focused) {
	if (focused == GLFW_FALSE) {
		static_cast<DesktopApi&>(desktop).LoseFocus();
	}
}

void KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
	KeyEvent key_event;
	key_event.key = AsKey(key);
	switch (action) {
	case GLFW_PRESS: key_event.type = KeyEvent::KeyDown; break;
	case GLFW_RELEASE: key_event.type = KeyEvent::KeyUp; break;
	default: return;
	}
	static_cast<DesktopApi&>(desktop).DispatchKeyEvent(key_event);
}

void CharCallback(GLFWwindow* glfw_window, unsigned int codepoint) {
	KeyEvent key_event;
	key_event.ch = to_u16pair(codepoint);
	key_event.type = KeyEvent::Char;
	static_cast<DesktopApi&>(desktop).DispatchKeyEvent(key_event);
}

void CharModsCallback(GLFWwindow* glfw_window, unsigned int codepoint, int mods) {
	//
}

} // namespace


Handle CreateWindow(Window& window, const u16string& title) {
	GLFWwindow* glfw_window = glfwCreateWindow(1, 1, as_char_str(to_u8string(title).c_str()), nullptr, nullptr);
	if (glfw_window == nullptr) {
		throw std::runtime_error("GLFW: create window error");
	}
	glfwSetWindowUserPointer(glfw_window, &window);

	glfwSetWindowCloseCallback(glfw_window, WindowCloseCallback);

	glfwSetWindowSizeCallback(glfw_window, WindowSizeCallback);
	glfwSetFramebufferSizeCallback(glfw_window, FramebufferSizeCallback);
	glfwSetWindowPosCallback(glfw_window, WindowPosCallback);
	glfwSetWindowContentScaleCallback(glfw_window, WindowContentScaleCallback);

	glfwSetWindowIconifyCallback(glfw_window, WindowIconifyCallback);
	glfwSetWindowMaximizeCallback(glfw_window, WindowMaximizeCallback);
	glfwSetWindowRefreshCallback(glfw_window, WindowRefreshCallback);

	glfwSetCursorEnterCallback(glfw_window, CursorEnterCallback);
	glfwSetCursorPosCallback(glfw_window, CursorPosCallback);
	glfwSetMouseButtonCallback(glfw_window, MouseButtonCallback);
	glfwSetScrollCallback(glfw_window, ScrollCallback);

	glfwSetWindowFocusCallback(glfw_window, WindowFocusCallback);
	glfwSetKeyCallback(glfw_window, KeyCallback);
	glfwSetCharCallback(glfw_window, CharCallback);
	glfwSetCharModsCallback(glfw_window, CharModsCallback);

	return glfw_window;
}

void DestroyWindow(Handle handle) { glfwDestroyWindow(AsGLFWWindow(handle)); }

Scale GetWindowScale(Handle handle) {
	float xscale = 1.0f, yscale = 1.0f;
	glfwGetWindowContentScale(AsGLFWWindow(handle), &xscale, &yscale);
	return Scale(xscale, yscale);
}

void SetWindowTitle(Handle handle, const u16string& title) { glfwSetWindowTitle(AsGLFWWindow(handle), as_char_str(to_u8string(title).c_str())); }

void SetWindowRegion(Handle handle, Rect region) {
	GLFWwindow* glfw_window = AsGLFWWindow(handle);
	glfwSetWindowPos(glfw_window, static_cast<int>(std::lround(region.point.x)), static_cast<int>(std::lround(region.point.y)));
	glfwSetWindowSize(glfw_window, static_cast<int>(std::lround(region.size.width)), static_cast<int>(std::lround(region.size.height)));
}

void SetWindowOpacity(Handle handle, uchar opacity) { glfwSetWindowOpacity(AsGLFWWindow(handle), opacity / 255.0f); }

void ShowWindow(Handle handle) { glfwShowWindow(AsGLFWWindow(handle)); }
void HideWindow(Handle handle) { glfwHideWindow(AsGLFWWindow(handle)); }
void MinimizeWindow(Handle handle) { glfwIconifyWindow(AsGLFWWindow(handle)); }
void MaximizeWindow(Handle handle) { glfwMaximizeWindow(AsGLFWWindow(handle)); }
void RestoreWindow(Handle handle) { glfwRestoreWindow(AsGLFWWindow(handle)); }

void CloseWindow(Handle handle) { glfwSetWindowShouldClose(AsGLFWWindow(handle), GLFW_TRUE); }

void RedrawWindowRegion(Handle handle, Rect region) {}

void SetWindowCapture(Handle handle) { glfwSetInputMode(AsGLFWWindow(handle), GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void ReleaseWindowCapture(Handle handle) { glfwSetInputMode(AsGLFWWindow(handle), GLFW_CURSOR, GLFW_CURSOR_NORMAL); static_cast<DesktopApi&>(desktop).LoseCapture(); }
void SetWindowFocus(Handle handle) { glfwFocusWindow(AsGLFWWindow(handle)); }

void ImeWindowEnable(Handle handle) {}
void ImeWindowDisable(Handle handle) {}
void ImeWindowSetPosition(Handle handle, Point point) {}


} // namespace ViewDesign

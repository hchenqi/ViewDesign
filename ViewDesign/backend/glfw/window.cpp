#include "ViewDesign/system/window.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/view/Desktop.h"

#include <ViewDesign/platform/glfw/window.h>
#include <ViewDesign/platform/glfw/context.h>
#include <ViewDesign/platform/glfw/key.h>

#ifdef VIEWDESIGN_PLATFORM_STB
#include <stb_image.h>
#endif


namespace ViewDesign {

struct WindowPrivateAccess : Window {
	using Window::HasParent;
	using Window::GetScale;
	using Window::SetPixelPoint;
	using Window::SetPixelSize;
	using Window::SetScale;
	using Window::State;
	using Window::SetState;
	using Window::Redraw;
};

struct DesktopPrivateAccess : Desktop {
	using Desktop::LoseWindowTrack;
	using Desktop::LoseWindowCapture;
	using Desktop::LoseWindowFocus;
	using Desktop::DispatchMouseEvent;
	using Desktop::DispatchKeyEvent;
};

using namespace GLFW;


namespace {

inline ref_ptr<WindowPrivateAccess> GetWindow(GLFWwindow* glfw_window) {
	return static_cast<ref_ptr<WindowPrivateAccess>>(glfwGetWindowUserPointer(glfw_window));
}

inline DesktopPrivateAccess& GetDesktop() {
	return static_cast<DesktopPrivateAccess&>(Desktop::Get());
}


void WindowCloseCallback(GLFWwindow* glfw_window) {}


void WindowSizeCallback(GLFWwindow* glfw_window, int width, int height) {
	//
}

void FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height) {
	GetWindow(glfw_window)->SetPixelSize(SizeU(width, height));
}

void WindowPosCallback(GLFWwindow* glfw_window, int xpos, int ypos) {
	GetWindow(glfw_window)->SetPixelPoint(PointI(xpos, ypos));
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
	GetWindow(glfw_window)->Redraw(rect_infinite);
}


Point cursor_position;

void CursorEnterCallback(GLFWwindow* glfw_window, int entered) {
	if (entered == GLFW_FALSE) {
		GetDesktop().LoseWindowTrack();
	}
}

void CursorPosCallback(GLFWwindow* glfw_window, double xpos, double ypos) {
	MouseEvent mouse_event;
	mouse_event.point = cursor_position = Point(xpos, ypos) / GetWindow(glfw_window)->GetScale();
	mouse_event.type = MouseEvent::Move;
	GetDesktop().DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
}

void MouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods) {
	MouseEvent mouse_event;
	mouse_event.point = cursor_position;
	mouse_event.ctrl = (mods & GLFW_MOD_CONTROL) != 0;
	mouse_event.shift = (mods & GLFW_MOD_SHIFT) != 0;
	mouse_event.alt = (mods & GLFW_MOD_ALT) != 0;
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
	GetDesktop().DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
}

void ScrollCallback(GLFWwindow* glfw_window, double xoffset, double yoffset) {
	MouseEvent mouse_event;
	mouse_event.point = cursor_position;
	mouse_event.ctrl = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
	mouse_event.shift = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
	mouse_event.alt = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
	if (yoffset != 0.0) {
		mouse_event.wheel_delta = yoffset * 120;
		mouse_event.type = MouseEvent::WheelVertical;
		GetDesktop().DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
	}
	if (xoffset != 0.0) {
		mouse_event.wheel_delta = xoffset * 120;
		mouse_event.type = MouseEvent::WheelHorizontal;
		GetDesktop().DispatchMouseEvent(*GetWindow(glfw_window), mouse_event);
	}
}


void WindowFocusCallback(GLFWwindow* glfw_window, int focused) {
	if (focused == GLFW_FALSE) {
		GetDesktop().LoseWindowFocus();
	}
}

void KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
	KeyEvent key_event;
	key_event.key = AsKey(key);
	switch (action) {
	case GLFW_REPEAT:
	case GLFW_PRESS: key_event.type = KeyEvent::KeyDown; break;
	case GLFW_RELEASE: key_event.type = KeyEvent::KeyUp; break;
	default: return;
	}
	key_event.ctrl = (mods & GLFW_MOD_CONTROL) != 0;
	key_event.shift = (mods & GLFW_MOD_SHIFT) != 0;
	key_event.alt = (mods & GLFW_MOD_ALT) != 0;
	GetDesktop().DispatchKeyEvent(key_event);
}

void CharCallback(GLFWwindow* glfw_window, unsigned int codepoint) {
	KeyEvent key_event;
	key_event.ch = to_u16pair(codepoint);
	key_event.type = KeyEvent::Char;
	key_event.ctrl = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
	key_event.shift = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
	key_event.alt = (glfwGetKey(glfw_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
	GetDesktop().DispatchKeyEvent(key_event);
}

void CharModsCallback(GLFWwindow* glfw_window, unsigned int codepoint, int mods) {
	//
}

} // namespace


Handle CreateWindow(const u16string& title) {
#if defined(VIEWDESIGN_BACKEND_GLFW_OPENGL)
	GLFWwindow* share = Context::GetHelperWindow();
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW_VULKAN)
	GLFWwindow* share = nullptr;
#endif
	GLFWwindow* glfw_window = glfwCreateWindow(1, 1, as_char_str(to_u8string(title).c_str()), nullptr, share);
	if (glfw_window == nullptr) { throw std::runtime_error("GLFW: create window error"); }
	return glfw_window;
}

void AttachWindow(Handle window, Window& view) {
	GLFWwindow* glfw_window = AsGLFWWindow(window);

	glfwSetWindowUserPointer(glfw_window, &view);

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
}

void DestroyWindow(Handle window) {
	if (Context::Initialized()) {
		glfwDestroyWindow(AsGLFWWindow(window));
	}
}

Scale GetWindowScale(Handle window) {
	float xscale = 1.0f, yscale = 1.0f;
	glfwGetWindowContentScale(AsGLFWWindow(window), &xscale, &yscale);
	return Scale(xscale, yscale);
}

void SetWindowTitle(Handle window, const u16string& title) { glfwSetWindowTitle(AsGLFWWindow(window), as_char_str(to_u8string(title).c_str())); }

void SetWindowRegion(Handle window, RectI region) {
	GLFWwindow* glfw_window = AsGLFWWindow(window);
	glfwSetWindowPos(glfw_window, region.point.x, region.point.y);
	glfwSetWindowSize(glfw_window, region.size.width, region.size.height);
}

void SetWindowOpacity(Handle window, float opacity) { glfwSetWindowOpacity(AsGLFWWindow(window), opacity); }

void SetWindowIcon(Handle window, const void* buffer, size_t size) {
#ifdef VIEWDESIGN_PLATFORM_STB
	int width, height, channels;
	stbi_uc* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer), (int)size, &width, &height, &channels, 4);
	if (data == nullptr) {
		throw std::runtime_error("GLFW: failed to decode icon image");
	}
	GLFWimage icon = { width, height, data };
	glfwSetWindowIcon(AsGLFWWindow(window), 1, &icon);
	stbi_image_free(data);
#else
	throw std::runtime_error("Window: setting an icon requires the Stb platform module");
#endif
}

void ClearWindowIcon(Handle window) { glfwSetWindowIcon(AsGLFWWindow(window), 0, nullptr); }

void ShowWindow(Handle window) { glfwShowWindow(AsGLFWWindow(window)); }
void HideWindow(Handle window) { glfwHideWindow(AsGLFWWindow(window)); }
void MinimizeWindow(Handle window) { glfwIconifyWindow(AsGLFWWindow(window)); }
void MaximizeWindow(Handle window) { glfwMaximizeWindow(AsGLFWWindow(window)); }
void RestoreWindow(Handle window) { glfwRestoreWindow(AsGLFWWindow(window)); }

void CloseWindow(Handle window) { glfwSetWindowShouldClose(AsGLFWWindow(window), GLFW_TRUE); }

void RedrawWindowRegion(Handle window, RectI region) {}

void SetWindowCapture(Handle window) { glfwSetInputMode(AsGLFWWindow(window), GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void ReleaseWindowCapture(Handle window) { glfwSetInputMode(AsGLFWWindow(window), GLFW_CURSOR, GLFW_CURSOR_NORMAL); GetDesktop().LoseWindowCapture(); }
void SetWindowFocus(Handle window) { glfwFocusWindow(AsGLFWWindow(window)); }

void ImeWindowEnable(Handle window) {}
void ImeWindowDisable(Handle window) {}
void ImeWindowSetPosition(Handle window, PointI point) {}


} // namespace ViewDesign

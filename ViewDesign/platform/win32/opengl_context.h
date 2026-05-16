#pragma once

#include "ViewDesign/platform/win32/context.h"


namespace ViewDesign {

namespace Win32 {


inline PIXELFORMATDESCRIPTOR PixelFormat() {
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	return pfd;
}


class OpenGLContext : public Context {
public:
	OpenGLContext() : Context() {
		helper_window_hdc = GetDC(helper_window);

		PIXELFORMATDESCRIPTOR pfd = PixelFormat();
		int pixelFormat = ChoosePixelFormat(helper_window_hdc, &pfd);
		SetPixelFormat(helper_window_hdc, pixelFormat, &pfd);

		helper_window_hglrc = wglCreateContext(helper_window_hdc);
		wglMakeCurrent(helper_window_hdc, helper_window_hglrc);
	}

	~OpenGLContext() {
		if (helper_window != nullptr) {
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(helper_window_hglrc); helper_window_hglrc = nullptr;
			ReleaseDC(helper_window, helper_window_hdc); helper_window_hdc = nullptr;
		}
	}

private:
	inline static HDC helper_window_hdc = nullptr;
	inline static HGLRC helper_window_hglrc = nullptr;
public:
	static HDC GetHelperWindowHDC() { if (!Initialized()) { throw std::invalid_argument("Win32: OpenGL context not initialized"); } return helper_window_hdc; }
	static HGLRC GetHelperWindowHGLRC() { if (!Initialized()) { throw std::invalid_argument("Win32: OpenGL context not initialized"); } return helper_window_hglrc; }
};


} // namespace Win32

} // namespace ViewDesign

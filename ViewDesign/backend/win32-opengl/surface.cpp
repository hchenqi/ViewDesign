#include "ViewDesign/drawing/surface.h"
#include "ViewDesign/platform/win32/window.h"
#include "ViewDesign/platform/win32/opengl_context.h"
#include "ViewDesign/platform/glad/canvas.h"

#include <GL/wglext.h>


namespace ViewDesign {

using namespace Win32;
using namespace OpenGL;


void Surface::Resize(SizeU size) {
	this->size = size;

	if (hglrc == nullptr) {
		HDC hdc = GetDC(AsHWND(window));

		PIXELFORMATDESCRIPTOR pfd = PixelFormat();
		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		static const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};
		HGLRC hglrc = wglCreateContextAttribsARB(hdc, OpenGLContext::GetHelperWindowHGLRC(), attribs);

		this->hdc = hdc;
		this->hglrc = hglrc;
	}

	invalid_region = invalid_region_front_buffer = RectI(point_i_zero, size);
}

void Surface::Destroy() {
	if (hglrc != nullptr) { wglDeleteContext((HGLRC)hglrc); hglrc = nullptr; }
	if (hdc != nullptr) { ReleaseDC(AsHWND(window), (HDC)hdc); hdc = nullptr; }
}

void Surface::RenderBegin() {
	if (!invalid_region.IsEmpty()) {
		wglMakeCurrent((HDC)hdc, (HGLRC)hglrc);
	}
}

void Surface::RenderEnd(const Canvas& canvas) {
	RenderContext context(size, nullptr);
	OpenGL::RenderCanvas(static_cast<RenderTarget&>(context), canvas, vector_zero, invalid_region);

	SwapBuffers((HDC)hdc);
	wglMakeCurrent(nullptr, nullptr);

	invalid_region = invalid_region_front_buffer;
	invalid_region_front_buffer = rect_i_empty;
}


} // namespace ViewDesign

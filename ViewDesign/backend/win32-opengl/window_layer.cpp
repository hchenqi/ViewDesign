#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/win32/window.h"
#include "ViewDesign/platform/win32/opengl_context.h"
#include "ViewDesign/platform/glad/texture.h"

#include <GL/wglext.h>


namespace ViewDesign {

using namespace Win32;


WindowLayer::WindowLayer() : window(nullptr), hdc(nullptr), hglrc(nullptr) {
	texture.Set(new TextureResource());
}

void WindowLayer::Create(Handle window, Size size) {
	Destroy();

	this->window = window;
	this->size = size;

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

void WindowLayer::Destroy() {
	if (hglrc != nullptr) { wglDeleteContext((HGLRC)hglrc); hglrc = nullptr; }
	if (hdc != nullptr) { ReleaseDC(AsHWND(window), (HDC)hdc); hdc = nullptr; }
}

void WindowLayer::Resize(Size size) {
	this->size = size;
}

void WindowLayer::CreateTexture() {}

void WindowLayer::Redraw(Rect redraw_region) {
	invalid_region = invalid_region.Union(redraw_region);
	invalid_region_front_buffer = invalid_region_front_buffer.Union(redraw_region);
}

void WindowLayer::RenderCanvas(const Canvas& canvas) {
	wglMakeCurrent((HDC)hdc, (HGLRC)hglrc);
	Layer::RenderCanvas(canvas, vector_zero, invalid_region);
}

void WindowLayer::Present() {
	SwapBuffers((HDC)hdc);
	invalid_region = invalid_region_front_buffer;
	invalid_region_front_buffer = region_empty;
}


} // namespace ViewDesign

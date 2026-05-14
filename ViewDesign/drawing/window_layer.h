#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/geometry/sizeu.h"
#include "ViewDesign/drawing/canvas.h"

#include <cassert>
#include <utility>


namespace ViewDesign {


class WindowLayer : Uncopyable {
public:
	WindowLayer(Handle window) : window(window) {}
	~WindowLayer() { assert(window == nullptr); } // DestroyWindow() should be called and the window handle should be manually destroyed before destruction

private:
	Handle window;
	SizeU size = size_u_empty;
public:
	Handle GetWindow() const { return window; }
	SizeU GetSize() const { return size; }
public:
	Handle DestroyWindow() { Destroy(); return std::exchange(window, nullptr); }

private:
#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
	Handle swap_chain = nullptr;
	Handle bitmap = nullptr;
	Handle composition_target = nullptr;
#endif
#if defined(VIEWDESIGN_BACKEND_WIN32_OPENGL)
	Handle hdc = nullptr;
	Handle hglrc = nullptr;
#endif
#if defined(VIEWDESIGN_BACKEND_WIN32_VULKAN)
	Handle surface = nullptr;
#endif
public:
	void Resize(SizeU size);
	void Destroy();

private:
	RectI invalid_region = rect_i_empty;
#if defined(VIEWDESIGN_BACKEND_OPENGL)
	RectI invalid_region_front_buffer = rect_i_empty;
#endif
public:
	void Redraw(RectI redraw_region) {
		invalid_region = invalid_region.Union(redraw_region);
#if defined(VIEWDESIGN_BACKEND_OPENGL)
		invalid_region_front_buffer = invalid_region_front_buffer.Union(redraw_region);
#endif
	}
private:
	void RenderBegin();
	void RenderEnd(const Canvas& canvas);
public:
	void Render(auto func) {
		RenderBegin();
		if (!invalid_region.IsEmpty()) {
			RenderEnd(func(invalid_region));
		}
	}
};


} // namespace ViewDesign

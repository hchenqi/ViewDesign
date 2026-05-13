#pragma once

#include "ViewDesign/drawing/layer.h"


namespace ViewDesign {


class WindowLayer : protected Layer {
public:
	WindowLayer();
	~WindowLayer() { Destroy(); }
private:
#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
	Handle swap_chain;
	Handle composition_target;
#endif
#if defined(VIEWDESIGN_BACKEND_WIN32_OPENGL)
	Handle window;
	Handle hdc;
	Handle hglrc;
#endif
#if defined(VIEWDESIGN_BACKEND_WIN32_VULKAN)
	Handle surface;
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW_OPENGL)
	Handle window;
#endif
public:
	void Create(Handle window, SizeU size);
	void Destroy();
	void Resize(SizeU size);
private:
	void CreateLayerFramebuffer(SizeU size);
	void DestroyLayerFramebuffer() { Layer::DestroyFramebuffer(); }
private:
	RectI invalid_region = rect_i_empty;
#if defined(VIEWDESIGN_BACKEND_OPENGL)
	RectI invalid_region_front_buffer = rect_i_empty;
#endif
public:
	RectI GetInvalidRegion() const { return invalid_region; }
	void Redraw(RectI redraw_region);
public:
	void RenderBegin();
	void RenderEnd(const Canvas& canvas);
};


} // namespace ViewDesign

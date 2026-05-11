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
	Handle window_surface;
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW_OPENGL)
	Handle window;
#endif
public:
	void Create(Handle window, Size size);
	void Destroy();
	void Resize(Size size);
private:
	void CreateLayerTexture(Size size);
	void DestroyLayerTexture() { Layer::DestroyTexture(); }
private:
	Rect invalid_region;
#if defined(VIEWDESIGN_BACKEND_OPENGL)
	Rect invalid_region_front_buffer;
#endif
public:
	Rect GetInvalidRegion() const { return invalid_region; }
	void Redraw(Rect redraw_region);
public:
	void RenderBegin();
	void RenderEnd(const Canvas& canvas);
};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/drawing/layer.h"


namespace ViewDesign {


class WindowLayer : private Layer {
public:
	WindowLayer();
	~WindowLayer() { Destroy(); }
private:
	using Handle = void*;
#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
private:
	Handle swap_chain;
	Handle comp_target;
#endif
#if defined(VIEWDESIGN_BACKEND_WIN32_OPENGL)
private:
	Handle window;
	Handle hdc;
	Handle hglrc;
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW_OPENGL)
private:
	Handle window;
#endif
public:
	void Create(Handle window, Size size);
	void Destroy();
	void Resize(Size size);
private:
	void CreateTexture();
	void DestroyTexture() { texture.Destroy(); }
private:
	Rect invalid_region;
#if defined(VIEWDESIGN_BACKEND_OPENGL)
	Rect invalid_region_front_buffer;
#endif
public:
	Rect GetInvalidRegion() const { return invalid_region; }
	void Redraw(Rect redraw_region);
public:
	void RenderCanvas(const Canvas& canvas);
	void Present();
};


} // namespace ViewDesign

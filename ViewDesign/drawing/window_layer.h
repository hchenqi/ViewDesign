#pragma once

#include "ViewDesign/drawing/layer.h"


namespace ViewDesign {

#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
struct SwapChain;
struct CompositionTarget;
#endif


class WindowLayer : private Layer {
public:
	WindowLayer();
	~WindowLayer() { Destroy(); }
private:
	using Handle = void*;
#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
private:
	owner_ptr<SwapChain> swap_chain;
	owner_ptr<CompositionTarget> comp_target;
#endif
public:
	void Create(Handle handle, Size size);
	void Destroy();
	void Resize(Size size);
private:
	void CreateTexture();
	void DestroyTexture() { texture.Destroy(); }
private:
	Rect invalid_region;
public:
	Rect GetInvalidRegion() const { return invalid_region; }
	void Redraw(Rect redraw_region);
public:
	void RenderCanvas(const Canvas& canvas);
	void Present();
};


} // namespace ViewDesign

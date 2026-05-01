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
public:
#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
	using Layer::RenderCanvas;
#endif
	void Present(Rect dirty_rect);
};


} // namespace ViewDesign

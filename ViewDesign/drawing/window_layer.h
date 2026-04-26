#pragma once

#include "ViewDesign/drawing/layer.h"


namespace ViewDesign {

struct SwapChain;
struct CompositionTarget;


class WindowLayer : private Layer {
public:
	WindowLayer() : swap_chain(nullptr), comp_target(nullptr) {}
	~WindowLayer() { Destroy(); }
private:
	using Handle = void*;
	owner_ptr<SwapChain> swap_chain;
	owner_ptr<CompositionTarget> comp_target;
public:
	void Create(Handle handle, Size size);
	void Destroy();
	void Resize(Size size);
private:
	void CreateTexture();
	void DestroyTexture() { texture.Destroy(); }
public:
	using Layer::RenderCanvas;
	void Present(Rect dirty_rect);
};


} // namespace ViewDesign

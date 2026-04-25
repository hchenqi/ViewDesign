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
	using HANDLE = void*;
	owner_ptr<SwapChain> swap_chain;
	owner_ptr<CompositionTarget> comp_target;
public:
	void Create(HANDLE hwnd, Size size);
	void Destroy();
	void Resize(Size size);
private:
	void CreateTexture();
	void DestroyTexture() { texture.Destroy(); }
public:
	using Layer::DrawCanvas;
	void Present(Rect dirty_rect);
};


} // namespace ViewDesign

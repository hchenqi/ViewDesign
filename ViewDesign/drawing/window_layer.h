#pragma once

#include "ViewDesign/drawing/layer.h"


namespace ViewDesign {

struct SwapChain;
struct CompositionTarget;


class WindowLayer : private Layer {
public:
	using HANDLE = void*;
	owner_ptr<SwapChain> swap_chain;
	owner_ptr<CompositionTarget> comp_target;
public:
	WindowLayer() : swap_chain(nullptr), comp_target(nullptr) {}
	~WindowLayer() { Destroy(); }
private:
	void CreateBitmap();
	void DestroyBitmap();
public:
	void Create(HANDLE hwnd, Size size);
	void Destroy();
	void Resize(Size size);
	void Present(Rect dirty_rect);
	using Layer::DrawCanvas;
};


} // namespace ViewDesign

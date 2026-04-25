#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/win32/d3d_api.h"
#include "ViewDesign/platform/win32/dxgi_api.h"
#include "ViewDesign/platform/win32/dcomp_api.h"
#include "ViewDesign/platform/win32/d2d_api.h"
#include "ViewDesign/platform/win32/directx_helper.h"


namespace ViewDesign {

using namespace Win32;


namespace {

inline owner_ptr<ID2D1Bitmap1> CreateD2DBitmapFromDxgiSurface(IDXGISurface& dxgi_surface) {
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<ID2D1Bitmap1> bitmap;
	hr << GetD2DDeviceContext().CreateBitmapFromDxgiSurface(&dxgi_surface, &bitmap_properties, &bitmap);
	return bitmap.Detach();
}

} // namespace


void WindowLayer::Create(HANDLE hwnd, Size size) {
	Destroy();

	// Create swapchain.
	ComPtr<IDXGISwapChain1> swap_chain;
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
	swap_chain_desc.Width = (uint)ceilf(size.width);
	swap_chain_desc.Height = (uint)ceilf(size.height);
	swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swap_chain_desc.Stereo = false;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swap_chain_desc.Flags = 0;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	hr << GetDXGIFactory().CreateSwapChainForComposition(&GetD3DDevice(), &swap_chain_desc, nullptr, &swap_chain);
	this->swap_chain = static_cast<owner_ptr<SwapChain>>(swap_chain.Detach());

	// Create texture.
	CreateTexture();

	// Create DComp resource.
	ComPtr<IDCompositionVisual> comp_visual;
	hr << GetDCompositionDevice().CreateVisual(&comp_visual);
	comp_visual->SetContent(this->swap_chain);

	ComPtr<IDCompositionTarget> comp_target;
	hr << GetDCompositionDevice().CreateTargetForHwnd((HWND)hwnd, false, &comp_target);
	comp_target->SetRoot(comp_visual.Get());
	this->comp_target = static_cast<owner_ptr<CompositionTarget>>(comp_target.Detach());

	GetDCompositionDevice().Commit();
}

void WindowLayer::Destroy() {
	SafeRelease(&comp_target);
	DestroyTexture();
	SafeRelease(&swap_chain);
}

void WindowLayer::Resize(Size size) {
	DestroyTexture();
	hr << swap_chain->ResizeBuffers(0, (uint)ceilf(size.width), (uint)ceilf(size.height), DXGI_FORMAT_UNKNOWN, 0);
	CreateTexture();
}

void WindowLayer::CreateTexture() {
	ComPtr<IDXGISurface> dxgi_surface;
	hr << swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));
	texture.Set(static_cast<owner_ptr<TextureResource>>(CreateD2DBitmapFromDxgiSurface(*dxgi_surface.Get())));
}

void WindowLayer::Present(Rect rect) {
	RECT dirty_rect = { (int)rect.left(), (int)rect.top(), (int)rect.right(), (int)rect.bottom() };
	DXGI_PRESENT_PARAMETERS present_parameters = { 1, &dirty_rect };
	hr << swap_chain->Present1(0, 0, &present_parameters);
}


} // namespace ViewDesign

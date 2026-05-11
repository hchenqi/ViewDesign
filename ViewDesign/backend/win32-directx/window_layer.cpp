#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/win32/window.h"
#include "ViewDesign/platform/win32/geometry_helper.h"
#include "ViewDesign/platform/directx/d3d_api.h"
#include "ViewDesign/platform/directx/dxgi_api.h"
#include "ViewDesign/platform/directx/dcomp_api.h"
#include "ViewDesign/platform/directx/d2d_api.h"
#include "ViewDesign/platform/directx/helper.h"
#include "ViewDesign/platform/directx/resource.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct DesktopPrivateAccess : Desktop {
	using Desktop::RecreateWindowLayer;
};

using namespace Win32;
using namespace DirectX;


namespace {

using SwapChain = DXGISwapChain;
using CompositionTarget = DCompositionTarget;

inline ComPtr<D2DBitmap> CreateD2DBitmapFromDxgiSurface(IDXGISurface& dxgi_surface) {
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<D2DBitmap> bitmap;
	hr << GetD2DDeviceContext().CreateBitmapFromDxgiSurface(&dxgi_surface, &bitmap_properties, &bitmap);
	return bitmap;
}

} // namespace


WindowLayer::WindowLayer() : swap_chain(nullptr), composition_target(nullptr) {}

void WindowLayer::Create(Handle window, Size size) {
	Destroy();

	ComPtr<SwapChain> swap_chain;
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

	this->swap_chain = swap_chain.Detach();

	CreateLayerTexture(size);

	DCompositionDevice& composition_device = GetDCompositionDevice();

	ComPtr<IDCompositionVisual> composition_visual;
	hr << composition_device.CreateVisual(&composition_visual);
	composition_visual->SetContent(static_cast<ref_ptr<SwapChain>>(this->swap_chain));

	ComPtr<IDCompositionTarget> composition_target;
	hr << composition_device.CreateTargetForHwnd(AsHWND(window), false, &composition_target);
	composition_target->SetRoot(composition_visual.Get());

	this->composition_target = composition_target.Detach();

	composition_device.Commit();
}

void WindowLayer::Destroy() {
	ComPtr<CompositionTarget>().Swap(reinterpret_cast<owner_ptr<CompositionTarget>&>(composition_target)).Reset();
	DestroyLayerTexture();
	ComPtr<SwapChain>().Swap(reinterpret_cast<owner_ptr<SwapChain>&>(swap_chain)).Reset();
}

void WindowLayer::Resize(Size size) {
	DestroyLayerTexture();
	hr << static_cast<ref_ptr<SwapChain>>(swap_chain)->ResizeBuffers(0, (uint)ceilf(size.width), (uint)ceilf(size.height), DXGI_FORMAT_UNKNOWN, 0);
	CreateLayerTexture(size);
}

void WindowLayer::CreateLayerTexture(Size size) {
	ComPtr<IDXGISurface> dxgi_surface;
	hr << static_cast<ref_ptr<SwapChain>>(swap_chain)->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));
	Layer::SetTexture(size, CreateD2DBitmapFromDxgiSurface(dxgi_surface).Detach());
	invalid_region = region_empty;
}

void WindowLayer::Redraw(Rect redraw_region) {
	invalid_region = invalid_region.Union(redraw_region);
}

void WindowLayer::RenderBegin() {
	GetD2DDeviceContext().BeginDraw();
}

void WindowLayer::RenderEnd(const Canvas& canvas) {
	Layer::RenderCanvas(canvas, vector_zero, invalid_region);
	try {
		hr << GetD2DDeviceContext().EndDraw();
	} catch (std::runtime_error&) {
		RecreateResource();
		static_cast<DesktopPrivateAccess&>(desktop.Get()).RecreateWindowLayer();
	}

	RECT dirty_rect = Win32::AsRECT(invalid_region);
	DXGI_PRESENT_PARAMETERS present_parameters = { 1, &dirty_rect };
	hr << static_cast<ref_ptr<SwapChain>>(swap_chain)->Present1(0, 0, &present_parameters);
	invalid_region = region_empty;
}


} // namespace ViewDesign

#include "ViewDesign/drawing/surface.h"
#include "ViewDesign/platform/win32/window.h"
#include "ViewDesign/platform/win32/geometry_helper.h"
#include "ViewDesign/platform/directx/d3d_api.h"
#include "ViewDesign/platform/directx/dxgi_api.h"
#include "ViewDesign/platform/directx/dcomp_api.h"
#include "ViewDesign/platform/directx/render_target.h"
#include "ViewDesign/platform/directx/helper.h"
#include "ViewDesign/platform/directx/resource.h"


namespace ViewDesign {

using namespace Win32;
using namespace DirectX;


namespace {

using SwapChain = DXGISwapChain;
using CompositionTarget = DCompositionTarget;

inline ComPtr<SwapChain> CreateSwapChainForComposition(SizeU size) {
	ComPtr<SwapChain> swap_chain;
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
	swap_chain_desc.Width = size.width;
	swap_chain_desc.Height = size.height;
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
	return swap_chain;
}

inline ComPtr<D2DBitmap> CreateBitmapFromDxgiSurface(IDXGISurface& dxgi_surface) {
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<D2DBitmap> bitmap;
	hr << GetD2DDeviceContext().CreateBitmapFromDxgiSurface(&dxgi_surface, &bitmap_properties, &bitmap);
	return bitmap;
}

inline ComPtr<D2DBitmap> CreateBitmapFromSwapChain(SwapChain& swap_chain) {
	ComPtr<IDXGISurface> dxgi_surface;
	hr << swap_chain.GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));
	return CreateBitmapFromDxgiSurface(dxgi_surface);
}

inline ComPtr<IDCompositionTarget> CreateCompositionTarget(SwapChain& swap_chain, HWND hwnd) {
	DCompositionDevice& composition_device = GetDCompositionDevice();

	ComPtr<IDCompositionVisual> composition_visual;
	hr << composition_device.CreateVisual(&composition_visual);
	composition_visual->SetContent(&swap_chain);

	ComPtr<IDCompositionTarget> composition_target;
	hr << composition_device.CreateTargetForHwnd(hwnd, false, &composition_target);
	composition_target->SetRoot(composition_visual.Get());

	composition_device.Commit();

	return composition_target;
}

} // namespace


void Surface::Resize(SizeU size) {
	this->size = size;
	if (swap_chain == nullptr) {
		swap_chain = CreateSwapChainForComposition(size).Detach();
		bitmap = CreateBitmapFromSwapChain(*static_cast<ref_ptr<SwapChain>>(swap_chain)).Detach(); RegisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(bitmap));
		composition_target = CreateCompositionTarget(*static_cast<ref_ptr<SwapChain>>(swap_chain), AsHWND(window)).Detach();
	} else {
		try {
			ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(bitmap)).Reset();
			hr << static_cast<ref_ptr<SwapChain>>(swap_chain)->ResizeBuffers(0, size.width, size.height, DXGI_FORMAT_UNKNOWN, 0);
			bitmap = CreateBitmapFromSwapChain(*static_cast<ref_ptr<SwapChain>>(swap_chain)).Detach();
		} catch (std::runtime_error&) {
			RecreateResource();
		}
	}
	invalid_region = RectI(point_i_zero, size);
}

void Surface::Destroy() {
	ComPtr<CompositionTarget>().Swap(reinterpret_cast<owner_ptr<CompositionTarget>&>(composition_target)).Reset();
	ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(bitmap)).Reset(); UnregisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(bitmap));
	ComPtr<SwapChain>().Swap(reinterpret_cast<owner_ptr<SwapChain>&>(swap_chain)).Reset();
}

void Surface::RenderBegin() {
	if (bitmap == nullptr) {
		Destroy();
		Resize(size);
	}
	if (!invalid_region.IsEmpty()) {
		D2DDeviceContext& device_context = GetD2DDeviceContext();
		device_context.BeginDraw();
	}
}

void Surface::RenderEnd(const Canvas& canvas) {
	D2DDeviceContext& device_context = GetD2DDeviceContext();
	device_context.SetTarget(static_cast<ref_ptr<D2DBitmap>>(bitmap));
	ViewDesign::RenderCanvas(static_cast<RenderTarget&>(device_context), canvas, vector_zero, invalid_region);
	device_context.SetTarget(nullptr);

	try {
		hr << device_context.EndDraw();
	} catch (std::runtime_error&) {
		RecreateResource();
		return;
	}

	RECT dirty_rect = AsWin32RECT(invalid_region);
	DXGI_PRESENT_PARAMETERS present_parameters = { 1, &dirty_rect };
	hr << static_cast<ref_ptr<SwapChain>>(swap_chain)->Present1(0, 0, &present_parameters);
	invalid_region = rect_i_empty;
}


} // namespace ViewDesign

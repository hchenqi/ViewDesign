#include "ViewDesign/platform/win32/d3d_api.h"
#include "ViewDesign/platform/win32/dxgi_api.h"
#include "ViewDesign/platform/win32/dcomp_api.h"
#include "ViewDesign/platform/win32/d2d_api.h"
#include "ViewDesign/platform/win32/dwrite_api.h"
#include "ViewDesign/platform/win32/wic_api.h"
#include "ViewDesign/platform/win32/directx_resource.h"
#include "ViewDesign/platform/win32/directx_helper.h"
#include "ViewDesign/drawing/texture.h"

#include <unordered_set>


namespace ViewDesign {

namespace {

using Win32::ComPtr;
using Win32::hr;


struct DirectXResource {
public:
	// D3D
	ComPtr<D3DDevice> d3d_device;

	// DXGI
	ComPtr<DXGIDevice> dxgi_device;
	ComPtr<DXGIAdapter> dxgi_adapter;
	ComPtr<DXGIFactory> dxgi_factory;

	// DComp
	ComPtr<DCompositionDevice> dcomp_device;

	// D2D
	ComPtr<D2DFactory> d2d_factory;
	ComPtr<D2DDeviceContext> d2d_device_context;
	ComPtr<D2DSolidColorBrush> d2d_solid_color_brush;

	// DWrite
	ComPtr<DWriteFactory> dwrite_factory;

	// WIC
	ComPtr<WICFactory> wic_factory;

	// Texture (D2D bitmap)
	std::unordered_set<ref_ptr<Texture>> texture_set;

public:
	DirectXResource();
	~DirectXResource();
public:
	void CreateDeviceIndependentResource();
	void DiscardDeviceIndependentResource();
	void CreateDeviceDependentResource();
	void DiscardDeviceDependentResource();
public:
	void RecreateDeviceDependentResource() {
		DiscardDeviceDependentResource();
		CreateDeviceDependentResource();
	}
};

DirectXResource directx_resource;


DirectXResource::DirectXResource() {
	hr << CoInitialize(nullptr);
	CreateDeviceIndependentResource();
	CreateDeviceDependentResource();
}

DirectXResource::~DirectXResource() {
	DiscardDeviceDependentResource();
	DiscardDeviceIndependentResource();
	CoUninitialize();
}

void DirectXResource::CreateDeviceIndependentResource() {
	// Create D2D factory.
	hr << D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(D2DFactory), &d2d_factory);

	// Create DWrite factory.
	hr << DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DWriteFactory), &dwrite_factory);

	// Create WIC factory.
	hr << CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory));
}

void DirectXResource::DiscardDeviceIndependentResource() {
	d2d_factory.Reset();
	dwrite_factory.Reset();
	wic_factory.Reset();
}

void DirectXResource::CreateDeviceDependentResource() {
	// Create D3D device.
	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL featureLevel;

	hr << D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT
#ifdef _DEBUG
		| D3D11_CREATE_DEVICE_DEBUG
#endif
		,
		featureLevels, ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&d3d_device,
		&featureLevel,
		nullptr
	);
	hr << d3d_device->QueryInterface(IID_PPV_ARGS(&dxgi_device));
	hr << dxgi_device->GetAdapter(&dxgi_adapter);
	hr << dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));

	// Create DComp device.
	hr << DCompositionCreateDevice(dxgi_device.Get(), IID_PPV_ARGS(&dcomp_device));

	// Create D2D device context.
	ComPtr<D2DDevice> d2d_device;
	hr << d2d_factory->CreateDevice(dxgi_device.Get(), &d2d_device);
	hr << d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d_device_context);

	// Create D2D brush.
	hr << d2d_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &d2d_solid_color_brush);
}

void DirectXResource::DiscardDeviceDependentResource() {
	for (auto texture : texture_set) { texture->Destroy(); }

	d2d_solid_color_brush.Reset();
	d2d_device_context.Reset();

	dcomp_device.Reset();

	dxgi_factory.Reset();
	dxgi_adapter.Reset();
	dxgi_device.Reset();
#ifdef _DEBUG
	ComPtr<D3DDebug> d3d_debug;
	hr << d3d_device.As(&d3d_debug);
	d3d_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	d3d_debug.Reset();
#endif
	d3d_device.Reset();
}


} // namespace

namespace Win32 {


void RegisterTexture(Texture& texture) { directx_resource.texture_set.insert(&texture); }
void UnregisterTexture(Texture& texture) { directx_resource.texture_set.erase(&texture); }

void DirectXRecreateResource() { directx_resource.RecreateDeviceDependentResource(); }


D3DDevice& GetD3DDevice() { return static_cast<D3DDevice&>(*directx_resource.d3d_device.Get()); }

DXGIDevice& GetDXGIDevice() { return static_cast<DXGIDevice&>(*directx_resource.dxgi_device.Get()); }
DXGIAdapter& GetDXGIAdapter() { return static_cast<DXGIAdapter&>(*directx_resource.dxgi_adapter.Get()); }
DXGIFactory& GetDXGIFactory() { return static_cast<DXGIFactory&>(*directx_resource.dxgi_factory.Get()); }

DCompositionDevice& GetDCompositionDevice() { return static_cast<DCompositionDevice&>(*directx_resource.dcomp_device.Get()); }

D2DFactory& GetD2DFactory() { return static_cast<D2DFactory&>(*directx_resource.d2d_factory.Get()); }
D2DDeviceContext& GetD2DDeviceContext() { return static_cast<D2DDeviceContext&>(*directx_resource.d2d_device_context.Get()); }
D2DSolidColorBrush& GetD2DSolidColorBrush() { return static_cast<D2DSolidColorBrush&>(*directx_resource.d2d_solid_color_brush.Get()); }

DWriteFactory& GetDWriteFactory() { return static_cast<DWriteFactory&>(*directx_resource.dwrite_factory.Get()); }

WICFactory& GetWICFactory() { return static_cast<WICFactory&>(*directx_resource.wic_factory.Get()); }


D2DSolidColorBrush& GetD2DSolidColorBrush(Color color) {
	D2DSolidColorBrush& brush = GetD2DSolidColorBrush();
	brush.SetColor(AsD2DColor(color));
	return brush;
}


void BeginDraw() {
	GetD2DDeviceContext().BeginDraw();
}

void EndDraw() {
	hr << GetD2DDeviceContext().EndDraw();
	GetD2DDeviceContext().SetTarget(nullptr);
}


} // namespace Win32

} // namespace ViewDesign

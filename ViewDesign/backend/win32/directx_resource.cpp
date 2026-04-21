#include "ViewDesign/backend/win32/d3d_api.h"
#include "ViewDesign/backend/win32/dxgi_api.h"
#include "ViewDesign/backend/win32/dcomp_api.h"
#include "ViewDesign/backend/win32/d2d_api.h"
#include "ViewDesign/backend/win32/dwrite_api.h"
#include "ViewDesign/backend/win32/wic_api.h"
#include "ViewDesign/backend/win32/directx_resource.h"
#include "ViewDesign/backend/win32/directx_helper.h"
#include "ViewDesign/drawing/bitmap.h"

#include <unordered_set>


namespace ViewDesign {


struct DirectXResource {
public:
	// D3D
	ComPtr<ID3D11Device> d3d_device;

	// DXGI
	ComPtr<IDXGIDevice1> dxgi_device;
	ComPtr<IDXGIAdapter> dxgi_adapter;
	ComPtr<IDXGIFactory2> dxgi_factory;

	// DComp
	ComPtr<IDCompositionDevice> dcomp_device;

	// D2D
	ComPtr<ID2D1Factory8> d2d_factory;
	ComPtr<ID2D1DeviceContext7> d2d_device_context;
	ComPtr<ID2D1SolidColorBrush> d2d_solid_color_brush;

	// DWrite
	ComPtr<IDWriteFactory7> dwrite_factory;

	// WIC
	ComPtr<IWICImagingFactory2> wic_factory;

	// Bitmap (D2D bitmap)
	std::unordered_set<ref_ptr<Bitmap>> bitmap_set;

public:
	DirectXResource();
	~DirectXResource();

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
	hr << D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory8), &d2d_factory);

	// Create DWrite factory.
	hr << DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), &dwrite_factory);

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
	ComPtr<ID2D1Device7> d2d_device;
	hr << d2d_factory->CreateDevice(dxgi_device.Get(), &d2d_device);
	hr << d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d_device_context);

	// Create D2D brush.
	hr << d2d_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &d2d_solid_color_brush);
}

void DirectXResource::DiscardDeviceDependentResource() {
	for (auto bitmap : bitmap_set) { bitmap->Destroy(); }

	d2d_solid_color_brush.Reset();
	d2d_device_context.Reset();

	dcomp_device.Reset();

	dxgi_factory.Reset();
	dxgi_adapter.Reset();
	dxgi_device.Reset();
#ifdef _DEBUG
	ComPtr<ID3D11Debug> d3d_debug;
	hr << d3d_device.As(&d3d_debug);
	d3d_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	d3d_debug.Reset();
#endif
	d3d_device.Reset();
}


void RegisterBitmap(Bitmap& bitmap) { directx_resource.bitmap_set.insert(&bitmap); }
void UnregisterBitmap(Bitmap& bitmap) { directx_resource.bitmap_set.erase(&bitmap); }

void DirectXRecreateResource() { directx_resource.RecreateDeviceDependentResource(); }

ID3D11Device& GetD3DDevice() { return *directx_resource.d3d_device.Get(); }

IDXGIDevice1& GetDXGIDevice() { return *directx_resource.dxgi_device.Get(); }
IDXGIAdapter& GetDXGIAdapter() { return *directx_resource.dxgi_adapter.Get(); }
IDXGIFactory2& GetDXGIFactory() { return *directx_resource.dxgi_factory.Get(); }

IDCompositionDevice& GetDCompDevice() { return *directx_resource.dcomp_device.Get(); }

ID2D1Factory8& GetD2DFactory() { return *directx_resource.d2d_factory.Get(); }
ID2D1DeviceContext7& GetD2DDeviceContext() { return *directx_resource.d2d_device_context.Get(); }
ID2D1SolidColorBrush& GetD2DSolidColorBrush() { return *directx_resource.d2d_solid_color_brush.Get(); }

IDWriteFactory7& GetDWriteFactory() { return *directx_resource.dwrite_factory.Get(); }

IWICImagingFactory2& GetWICFactory() { return *directx_resource.wic_factory.Get(); }


ID2D1SolidColorBrush& GetD2DSolidColorBrush(Color color) {
	ID2D1SolidColorBrush& brush = GetD2DSolidColorBrush();
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


} // namespace ViewDesign

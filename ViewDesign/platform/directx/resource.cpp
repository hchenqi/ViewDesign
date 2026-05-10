#define NOMINMAX
#include "ViewDesign/platform/directx/d3d_api.h"
#include "ViewDesign/platform/directx/dxgi_api.h"
#include "ViewDesign/platform/directx/dcomp_api.h"
#include "ViewDesign/platform/directx/d2d_api.h"
#include "ViewDesign/platform/directx/dwrite_api.h"
#include "ViewDesign/platform/directx/wic_api.h"
#include "ViewDesign/platform/directx/resource.h"
#include "ViewDesign/platform/directx/helper.h"

#include <unordered_set>


namespace ViewDesign {

namespace DirectX {

namespace {

struct Resource {
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

	// Bitmap
	std::unordered_set<ref_ptr<owner_ptr<D2DBitmap>>> bitmap_set;

private:
	Resource() {
		hr << CoInitialize(nullptr);
		CreateDeviceIndependentResource();
		CreateDeviceDependentResource();
	}

	~Resource() {
		DestroyDeviceDependentResource();
		DestroyDeviceIndependentResource();
		CoUninitialize();
	}

public:
	static Resource& Get() {
		static Resource resource;
		return resource;
	}

public:
	void CreateDeviceIndependentResource() {
		hr << D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(D2DFactory), &d2d_factory);
		hr << DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DWriteFactory), &dwrite_factory);
		hr << CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory));
	}

	void DestroyDeviceIndependentResource() {
		wic_factory.Reset();
		dwrite_factory.Reset();
		d2d_factory.Reset();
	}

	void CreateDeviceDependentResource() {
		static constexpr D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
		};

		D3D_FEATURE_LEVEL featureLevel;

		hr << D3D11CreateDevice(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT
#if !defined(NDEBUG)
			| D3D11_CREATE_DEVICE_DEBUG
#endif
			,
			featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
			&d3d_device, &featureLevel, nullptr
		);
		hr << d3d_device->QueryInterface(IID_PPV_ARGS(&dxgi_device));
		hr << dxgi_device->GetAdapter(&dxgi_adapter);
		hr << dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));

		hr << DCompositionCreateDevice(dxgi_device.Get(), IID_PPV_ARGS(&dcomp_device));

		ComPtr<D2DDevice> d2d_device;
		hr << d2d_factory->CreateDevice(dxgi_device.Get(), &d2d_device);
		hr << d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d_device_context);

		hr << d2d_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &d2d_solid_color_brush);
	}

	void DestroyDeviceDependentResource() {
		for (auto bitmap : bitmap_set) {
			ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(*bitmap)).Reset();
		}

		d2d_solid_color_brush.Reset();
		d2d_device_context.Reset();

		dcomp_device.Reset();

		dxgi_factory.Reset();
		dxgi_adapter.Reset();
		dxgi_device.Reset();

#if !defined(NDEBUG)
		ComPtr<D3DDebug> d3d_debug;
		hr << d3d_device.As(&d3d_debug);
		d3d_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		d3d_debug.Reset();
#endif

		d3d_device.Reset();
	}

public:
	void RecreateDeviceDependentResource() {
		DestroyDeviceDependentResource();
		CreateDeviceDependentResource();
	}
};


} // namespace


D3DDevice& GetD3DDevice() { return Resource::Get().d3d_device; }

DXGIDevice& GetDXGIDevice() { return Resource::Get().dxgi_device; }
DXGIAdapter& GetDXGIAdapter() { return Resource::Get().dxgi_adapter; }
DXGIFactory& GetDXGIFactory() { return Resource::Get().dxgi_factory; }

DCompositionDevice& GetDCompositionDevice() { return Resource::Get().dcomp_device; }

D2DFactory& GetD2DFactory() { return Resource::Get().d2d_factory; }
D2DDeviceContext& GetD2DDeviceContext() { return Resource::Get().d2d_device_context; }
D2DSolidColorBrush& GetD2DSolidColorBrush() { return Resource::Get().d2d_solid_color_brush; }

void RegisterBitmap(owner_ptr<D2DBitmap>& bitmap) { Resource::Get().bitmap_set.insert(&bitmap); }
void UnregisterBitmap(owner_ptr<D2DBitmap>& bitmap) { Resource::Get().bitmap_set.erase(&bitmap); }

DWriteFactory& GetDWriteFactory() { return Resource::Get().dwrite_factory; }

WICFactory& GetWICFactory() { return Resource::Get().wic_factory; }


void RecreateResource() { Resource::Get().RecreateDeviceDependentResource(); }


} // namespace DirectX

} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/drawing/color.h"

#include <d2d1_3.h>
#include <dxgi1_4.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwrite_3.h>
#include <wincodec.h>

#include <wrl/client.h>

#include <stdexcept>


using D3DDevice = ID3D11Device;
using D3DDebug = ID3D11Debug;
using DXGIDevice = IDXGIDevice1;
using DXGIAdapter = IDXGIAdapter;
using DXGIFactory = IDXGIFactory2;
using DXGISwapChain = IDXGISwapChain1;
using DCompositionDevice = IDCompositionDevice;
using DCompositionTarget = IDCompositionTarget;
using D2DFactory = ID2D1Factory8;
using D2DDevice = ID2D1Device7;
using D2DDeviceContext = ID2D1DeviceContext7;
using D2DSolidColorBrush = ID2D1SolidColorBrush;
using D2DBitmap = ID2D1Bitmap1;
using DWriteFactory = IDWriteFactory7;
using DWriteTextLayout = IDWriteTextLayout4;
using WICFactory = IWICImagingFactory2;
using WICFormatConverter = IWICFormatConverter;


namespace ViewDesign {


using Microsoft::WRL::ComPtr;


struct HResultWrapper {
	HResultWrapper& operator<<(HRESULT hr) {
		if (!SUCCEEDED(hr)) { throw std::runtime_error("hresult failure"); }
		return *this;
	}
};

inline static HResultWrapper hr;


template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != nullptr) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}


struct D3DDevice : ::D3DDevice {};
struct DXGIDevice : ::DXGIDevice {};
struct DXGIAdapter : ::DXGIAdapter {};
struct DXGIFactory : ::DXGIFactory {};
struct DCompositionDevice : ::DCompositionDevice {};
struct D2DFactory : ::D2DFactory {};
struct D2DDeviceContext : ::D2DDeviceContext {};
struct D2DSolidColorBrush : ::D2DSolidColorBrush {};
struct DWriteFactory : ::DWriteFactory {};
struct WICFactory : ::WICFactory {};

struct SwapChain : DXGISwapChain {};
struct CompositionTarget : DCompositionTarget {};
struct RenderTarget : D2DDeviceContext {};
struct BitmapResource : D2DBitmap {};
struct TextLayout : DWriteTextLayout {};
struct ImageSource : WICFormatConverter {};


inline D2D1_POINT_2F AsD2DPoint(Point point) { return { point.x, point.y }; }
inline D2D1_SIZE_F AsD2DSize(Size size) { return { size.width, size.height }; }
inline D2D1_RECT_F AsD2DRect(Rect rect) { return { rect.left(), rect.top(), rect.right(), rect.bottom() }; }
inline Point AsPoint(D2D1_POINT_2F point) { return Point(point.x, point.y); }
inline Size AsSize(D2D1_SIZE_F size) { return Size(size.width, size.height); }
inline Rect AsRect(D2D1_RECT_F rect) { return Rect(Point(rect.left, rect.top), Size(rect.right - rect.left, rect.bottom - rect.top)); }

inline D2D1::Matrix3x2F AsD2DTransform(Transform transform) { return reinterpret_cast<const D2D1::Matrix3x2F&>(transform); }
inline Transform AsTransform(D2D1::Matrix3x2F matrix) { return reinterpret_cast<const Transform&>(matrix); }

inline float OpacityAsFloat(uchar opacity) { return opacity / (float)0xFF; }
inline D2D1_COLOR_F AsD2DColor(Color color) { return D2D1::ColorF(color.AsUnsigned(), OpacityAsFloat(color.alpha)); }


} // namespace ViewDesign

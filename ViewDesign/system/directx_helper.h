#pragma once

#include "../geometry/geometry.h"
#include "../geometry/transform.h"
#include "../figure/color.h"

#include <d2d1_3.h>
#include <dxgi1_4.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwrite_3.h>
#include <wincodec.h>

#include <wrl/client.h>


namespace ViewDesign {


using Microsoft::WRL::ComPtr;


struct HResultWrapper {
	HRESULT hr = 0;

	HResultWrapper() = default;
	HResultWrapper(HRESULT hr) : hr(hr) {}

	HResultWrapper& operator<<(HRESULT hr) {
		this->hr = hr;
		if (!SUCCEEDED(hr)) { throw std::runtime_error("hresult failure"); }
		return *this;
	}

	operator HRESULT() const { return hr; }
};

inline static HResultWrapper hr;


template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != nullptr) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}


// resource aliases
struct RenderTarget : ID2D1DeviceContext7 {};
struct BitmapResource : ID2D1Bitmap1 {};
struct SwapChain : IDXGISwapChain1 {};
struct CompositionTarget : IDCompositionTarget {};
struct ImageSource : IWICFormatConverter {};
struct TextLayout : IDWriteTextLayout4 {};


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

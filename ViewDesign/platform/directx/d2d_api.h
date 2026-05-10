#pragma once

#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/drawing/color.h"

#include <d2d1_1.h>


namespace ViewDesign {

namespace DirectX {

using D2DFactory = ID2D1Factory1;
using D2DDevice = ID2D1Device;
using D2DDeviceContext = ID2D1DeviceContext;
using D2DSolidColorBrush = ID2D1SolidColorBrush;
using D2DBitmap = ID2D1Bitmap1;


D2DFactory& GetD2DFactory();
D2DDeviceContext& GetD2DDeviceContext();
D2DSolidColorBrush& GetD2DSolidColorBrush();

void RegisterBitmap(owner_ptr<D2DBitmap>& bitmap);
void UnregisterBitmap(owner_ptr<D2DBitmap>& bitmap);


inline D2D1_POINT_2F AsD2DPoint(Point point) { return { point.x, point.y }; }
inline D2D1_SIZE_F AsD2DSize(Size size) { return { size.width, size.height }; }
inline D2D1_RECT_F AsD2DRect(Rect rect) { return { rect.left(), rect.top(), rect.right(), rect.bottom() }; }
inline Point AsPoint(D2D1_POINT_2F point) { return Point(point.x, point.y); }
inline Size AsSize(D2D1_SIZE_F size) { return Size(size.width, size.height); }
inline Rect AsRect(D2D1_RECT_F rect) { return Rect(Point(rect.left, rect.top), Size(rect.right - rect.left, rect.bottom - rect.top)); }

inline D2D1::Matrix3x2F AsD2DTransform(Transform transform) { return reinterpret_cast<const D2D1::Matrix3x2F&>(transform); }
inline Transform AsTransform(D2D1::Matrix3x2F matrix) { return reinterpret_cast<const Transform&>(matrix); }

inline D2D1_COLOR_F AsD2DColor(Color color) { return D2D1::ColorF(color.AsUnsigned(), color.alpha / (float)0xFF); }


inline D2DSolidColorBrush& GetD2DSolidColorBrush(Color color) {
	D2DSolidColorBrush& brush = GetD2DSolidColorBrush();
	brush.SetColor(AsD2DColor(color));
	return brush;
}


} // namespace DirectX


struct RenderTarget : DirectX::D2DDeviceContext {};


} // namespace ViewDesign

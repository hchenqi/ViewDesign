#include "ViewDesign/drawing/shape.h"
#include "ViewDesign/backend/win32/d2d_api.h"
#include "ViewDesign/backend/win32/directx_helper.h"


namespace ViewDesign {


inline D2D1_RECT_F ShrinkD2DRect(D2D1_RECT_F rect, float length) {
	return D2D1::RectF(rect.left + length, rect.top + length, rect.right - length, rect.bottom - length);
}


void Line::DrawOn(RenderTarget& target, Point point) const {
	if (width > 0.0f && color.IsVisible()) {
		target.DrawLine(AsD2DPoint(point + begin), AsD2DPoint(point + end), &GetD2DSolidColorBrush(color), width);
	}
}


void Rectangle::DrawOn(RenderTarget& target, Point point) const {
	if (fill_color.IsVisible()) {
		target.FillRectangle(AsD2DRect(Rect(point, size)), &GetD2DSolidColorBrush(fill_color));
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		target.DrawRectangle(ShrinkD2DRect(AsD2DRect(Rect(point, size)), border_width / 2.0f), &GetD2DSolidColorBrush(border_color), border_width);
	}
}


void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {
	if (fill_color.IsVisible()) {
		target.FillRoundedRectangle(D2D1::RoundedRect(AsD2DRect(Rect(point, size)), radius, radius), &GetD2DSolidColorBrush(fill_color));
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		target.DrawRoundedRectangle(D2D1::RoundedRect(ShrinkD2DRect(AsD2DRect(Rect(point, size)), border_width / 2.0f), radius, radius), &GetD2DSolidColorBrush(border_color), border_width);
	}
}


void Ellipse::DrawOn(RenderTarget& target, Point point) const {
	if (fill_color.IsVisible()) {
		target.FillEllipse(D2D1::Ellipse(AsD2DPoint(point), radius_x, radius_y), &GetD2DSolidColorBrush(fill_color));
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		target.DrawEllipse(D2D1::Ellipse(AsD2DPoint(point), radius_x - border_width / 2.0f, radius_y - border_width / 2.0f), &GetD2DSolidColorBrush(border_color), border_width);
	}
}


} // namespace ViewDesign

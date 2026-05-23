#pragma once

#include "ViewDesign/view/style/layout_value.h"
#include "ViewDesign/drawing/color.h"


namespace ViewDesign {


struct PositionStyle {
	LayoutValue _left;
	LayoutValue _top;
	LayoutValue _right;
	LayoutValue _bottom;

	constexpr PositionStyle() : PositionStyle(position_auto, position_auto, position_auto, position_auto) {}
	constexpr PositionStyle(LayoutValue all) : PositionStyle(all, all, all, all) {}
	constexpr PositionStyle(LayoutValue left_right, LayoutValue top_bottom) : PositionStyle(left_right, top_bottom, left_right, top_bottom) {}
	constexpr PositionStyle(LayoutValue left_right, LayoutValue top, LayoutValue bottom) : PositionStyle(left_right, top, left_right, bottom) {}
	constexpr PositionStyle(LayoutValue left, LayoutValue top, LayoutValue right, LayoutValue bottom) : _left(left), _top(top), _right(right), _bottom(bottom) {}

	constexpr PositionStyle& left(LayoutValue left) { _left = left; return *this; }
	constexpr PositionStyle& top(LayoutValue top) { _top = top; return *this; }
	constexpr PositionStyle& right(LayoutValue right) { _right = right; return *this; }
	constexpr PositionStyle& bottom(LayoutValue bottom) { _bottom = bottom; return *this; }
	constexpr void set(LayoutValue left, LayoutValue top, LayoutValue right, LayoutValue bottom) { _left = left; _top = top; _right = right; _bottom = bottom; }
	constexpr void setAll(LayoutValue all) { _left = _top = _right = _bottom = all; }
	constexpr PositionStyle& setHorizontalCenter() { _left = _right = position_center; return *this; }
	constexpr PositionStyle& setVerticalCenter() { _top = _bottom = position_center; return *this; }
};


} // namespace ViewDesign

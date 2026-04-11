#pragma once

#include "value_tag.h"
#include "../figure/color.h"


namespace ViewDesign {


struct LengthStyle {
	ValueTag _normal;
	ValueTag _min;
	ValueTag _max;

	constexpr LengthStyle() : LengthStyle(length_auto, length_min_tag, length_max_tag) {}
	constexpr LengthStyle(ValueTag normal) : LengthStyle(normal, normal, normal) {}
	constexpr LengthStyle(ValueTag normal, ValueTag min, ValueTag max) : _normal(normal), _min(min), _max(max) {}

	constexpr LengthStyle& normal(ValueTag normal) { _normal = normal; return *this; }
	constexpr LengthStyle& min(ValueTag min) { _min = min; return *this; }
	constexpr LengthStyle& max(ValueTag max) { _max = max; return *this; }
	constexpr void setFixed(ValueTag length) { _normal = _min = _max = length; }
};


struct PositionStyle {
	ValueTag _left;
	ValueTag _top;
	ValueTag _right;
	ValueTag _bottom;

	constexpr PositionStyle() : PositionStyle(position_auto, position_auto, position_auto, position_auto) {}
	constexpr PositionStyle(ValueTag all) : PositionStyle(all, all, all, all) {}
	constexpr PositionStyle(ValueTag left_right, ValueTag top_bottom) : PositionStyle(left_right, top_bottom, left_right, top_bottom) {}
	constexpr PositionStyle(ValueTag left_right, ValueTag top, ValueTag bottom) : PositionStyle(left_right, top, left_right, bottom) {}
	constexpr PositionStyle(ValueTag left, ValueTag top, ValueTag right, ValueTag bottom) : _left(left), _top(top), _right(right), _bottom(bottom) {}

	constexpr PositionStyle& left(ValueTag left) { _left = left; return *this; }
	constexpr PositionStyle& top(ValueTag top) { _top = top; return *this; }
	constexpr PositionStyle& right(ValueTag right) { _right = right; return *this; }
	constexpr PositionStyle& bottom(ValueTag bottom) { _bottom = bottom; return *this; }
	constexpr void set(ValueTag left, ValueTag top, ValueTag right, ValueTag bottom) { _left = left; _top = top; _right = right; _bottom = bottom; }
	constexpr void setAll(ValueTag all) { _left = _top = _right = _bottom = all; }
	constexpr PositionStyle& setHorizontalCenter() { _left = _right = position_center; return *this; }
	constexpr PositionStyle& setVerticalCenter() { _top = _bottom = position_center; return *this; }
};


} // namespace ViewDesign

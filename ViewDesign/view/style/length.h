#pragma once

#include "ViewDesign/view/style/layout_value.h"


namespace ViewDesign {


struct LengthStyle {
	LayoutValue _normal;
	LayoutValue _min;
	LayoutValue _max;

	constexpr LengthStyle() : LengthStyle(length_auto, length_zero_value, length_infinite_value) {}
	constexpr LengthStyle(LayoutValue normal) : LengthStyle(normal, normal, normal) {}
	constexpr LengthStyle(LayoutValue normal, LayoutValue min, LayoutValue max) : _normal(normal), _min(min), _max(max) {}

	constexpr LengthStyle& normal(LayoutValue normal) { _normal = normal; return *this; }
	constexpr LengthStyle& min(LayoutValue min) { _min = min; return *this; }
	constexpr LengthStyle& max(LayoutValue max) { _max = max; return *this; }
	constexpr void fix(LayoutValue length) { _normal = _min = _max = length; }
};


} // namespace ViewDesign

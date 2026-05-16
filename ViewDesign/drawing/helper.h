#pragma once

#include "ViewDesign/geometry/rect.h"
#include "ViewDesign/drawing/color.h"


namespace ViewDesign {


inline std::tuple<float, float, float, float> AsTuple(Rect rect) {
	return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

inline std::tuple<float, float, float, float> AsTupleNormalized(Size size, Rect rect) {
	return { rect.left() / size.width, rect.top() / size.height, rect.right() / size.width, rect.bottom() / size.height };
}


inline Color AsPremultiplied(Color color) {
	color.blue = (color.blue * color.alpha + 0x7F) / 0xFF;
	color.green = (color.green * color.alpha + 0x7F) / 0xFF;
	color.red = (color.red * color.alpha + 0x7F) / 0xFF;
	return color;
}

inline std::tuple<float, float, float, float> AsTupleNormalizedPremultiplied(Color color) {
	float alpha = color.alpha / 255.0f;
	return { color.red * alpha / 255.0f, color.green * alpha / 255.0f, color.blue * alpha / 255.0f, alpha };
}


} // namespace ViewDesign

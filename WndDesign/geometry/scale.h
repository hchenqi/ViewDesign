#pragma once

#include "../common/core.h"


namespace ViewDesign {


struct Scale {
	float x;
	float y;

	constexpr Scale() : Scale(1.0f) {}
	constexpr Scale(float scale) : Scale(scale, scale) {}
	constexpr Scale(float x, float y) : x(x), y(y) {}

	constexpr bool operator==(const Scale& scale) const { return x == scale.x && y == scale.y; }
	constexpr bool operator!=(const Scale& scale) const { return x != scale.x || y != scale.y; }

	constexpr Scale operator*(const Scale& scale) const { return Scale(x * scale.x, y * scale.y); }

	constexpr Scale Invert() const { return Scale(1.0f / x, 1.0f / y); }
};


} // namespace ViewDesign

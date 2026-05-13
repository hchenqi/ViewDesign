#pragma once


namespace ViewDesign {


struct Scale {
	float x;
	float y;

	constexpr Scale() = default;
	constexpr Scale(float scale) : Scale(scale, scale) {}
	constexpr Scale(float x, float y) : x(x), y(y) {}

	constexpr bool operator==(const Scale& other) const { return x == other.x && y == other.y; }

	constexpr Scale operator*(const Scale& other) const { return Scale(x * other.x, y * other.y); }
	constexpr Scale operator/(const Scale& other) const { return Scale(x / other.x, y / other.y); }

	constexpr Scale invert() const { return Scale(1.0f) / *this; }
};


} // namespace ViewDesign

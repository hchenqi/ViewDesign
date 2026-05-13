#pragma once


namespace ViewDesign {


struct Margin {
	float left;
	float top;
	float right;
	float bottom;

	constexpr Margin() : Margin(0, 0, 0, 0) {}
	constexpr Margin(float all) : Margin(all, all, all, all) {}
	constexpr Margin(float left_right, float top_bottom) : Margin(left_right, top_bottom, left_right, top_bottom) {}
	constexpr Margin(float left_right, float top, float bottom) : Margin(left_right, top, left_right, bottom) {}
	constexpr Margin(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom) {}

	constexpr Margin operator-() const { return Margin(-left, -top, -right, -bottom); }

	constexpr Margin operator+(const Margin& other) const {
		return Margin(left + other.left, top + other.top, right + other.right, bottom + other.bottom);
	}
};


} // namespace ViewDesign

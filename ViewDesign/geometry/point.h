#pragma once


namespace ViewDesign {


struct Point {
	float x;
	float y;

	explicit constexpr Point() : x(), y() {}
	explicit constexpr Point(float x, float y) : x(x), y(y) {}

	constexpr bool operator==(const Point& point) const { return x == point.x && y == point.y; }
	constexpr bool operator!=(const Point& point) const { return x != point.x || y != point.y; }

	constexpr bool operator>(const Point& point) const { return x > point.x && y > point.y; }
	constexpr bool operator<(const Point& point) const { return x < point.x && y < point.y; }
	constexpr bool operator>=(const Point& point) const { return x >= point.x && y >= point.y; }
	constexpr bool operator<=(const Point& point) const { return x <= point.x && y <= point.y; }
};


} // namespace ViewDesign

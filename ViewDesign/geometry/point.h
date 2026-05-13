#pragma once


namespace ViewDesign {


struct Point {
	float x;
	float y;

	explicit constexpr Point() = default;
	explicit constexpr Point(float x, float y) : x(x), y(y) {}

	constexpr bool operator==(const Point& other) const { return x == other.x && y == other.y; }

	constexpr bool operator>(const Point& other) const { return x > other.x && y > other.y; }
	constexpr bool operator<(const Point& other) const { return x < other.x && y < other.y; }
	constexpr bool operator>=(const Point& other) const { return x >= other.x && y >= other.y; }
	constexpr bool operator<=(const Point& other) const { return x <= other.x && y <= other.y; }
};


} // namespace ViewDesign

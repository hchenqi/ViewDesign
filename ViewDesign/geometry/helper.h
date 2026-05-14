#pragma once

#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/geometry/margin.h"


namespace ViewDesign {


constexpr float Clamp(float position, Interval interval) {
	return std::clamp(position, interval.left(), interval.right());
}

constexpr Point Clamp(Point point, Rect region) {
	return Point(Clamp(point.x, Interval(region.point.x, region.size.width)), Clamp(point.y, Interval(region.point.y, region.size.height)));
}

constexpr Interval Clamp(Interval interval1, Interval interval2) {
	interval1.length = std::clamp(interval1.length, length_zero, interval2.length);
	interval1.begin = std::clamp(interval1.begin, interval2.begin, interval2.right() - interval1.length);
	return interval1;
}

constexpr Rect Clamp(Rect rect, Rect region) {
	return Rect(Clamp(rect.Horizontal(), region.Horizontal()), Clamp(rect.Vertical(), region.Vertical()));
}


constexpr Size Extend(Size size, float length) {
	return Size(std::max(size.width + length, 0.0f), std::max(size.height + length, 0.0f));
}

constexpr Rect Extend(Rect region, float length) {
	return Rect(region.point - Vector(length, length), Extend(region.size, length * 2));
}

constexpr Size Extend(Size size, Margin margin) {
	return Size(std::max(size.width + margin.left + margin.right, 0.0f), std::max(size.height + margin.top + margin.bottom, 0.0f));
}

constexpr Rect Extend(Rect region, Margin margin) {
	return Rect(region.point - Vector(margin.left, margin.top), Extend(region.size, margin));
}


constexpr float square(float x) { return x * x; };

constexpr float square_distance(Point a, Point b) { return square(a.x - b.x) + square(a.y - b.y); }

constexpr bool PointInCircle(Point point, Point center, float radius) {
	return square_distance(point, center) <= square(radius);
}

constexpr bool PointInRoundedRectangle(Point point, Rect rect, float radius) {
	if (!rect.Contains(point)) { return false; } if (radius == 0.0f) { return true; }
	float x1 = rect.left() + radius, x2 = rect.right() - radius;
	float y1 = rect.top() + radius, y2 = rect.bottom() - radius;
	float x = point.x, y = point.y;
	if (x < x1 && y < y1) { return square(x - x1) + square(y - y1) <= square(radius); }
	if (x < x1 && y > y2) { return square(x - x1) + square(y - y2) <= square(radius); }
	if (x > x2 && y < y1) { return square(x - x2) + square(y - y1) <= square(radius); }
	if (x > x2 && y > y2) { return square(x - x2) + square(y - y2) <= square(radius); }
	return true;
}


} // namespace ViewDesign

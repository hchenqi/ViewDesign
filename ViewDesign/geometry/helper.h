#pragma once

#include "ViewDesign/geometry/geometry.h"
#include "ViewDesign/geometry/margin.h"


namespace ViewDesign {


constexpr Size Extend(Size size, float length) {
	float width = size.width + length * 2, height = size.height + length * 2;
	return Size(width >= 0.0f ? width : 0.0f, height >= 0.0f ? height : 0.0f);
}

constexpr Rect Extend(Rect region, float length) {
	return Rect(region.point - Vector(length, length), Extend(region.size, length));
}

constexpr Size Extend(Size size, Margin margin) {
	float width = size.width + margin.left + margin.right;
	float height = size.height + margin.top + margin.bottom;
	return Size(width >= 0.0f ? width : 0.0f, height >= 0.0f ? height : 0.0f);
}

constexpr Rect Extend(Rect region, Margin margin) {
	return Rect(region.point - Vector(margin.left, margin.top), Extend(region.size, margin));
}


constexpr Rect RoundUp(Rect region) {
	return Rect(floorf(region.point.x), floorf(region.point.y), ceilf(region.size.width), ceilf(region.size.height));
}


constexpr float square(float x) { return x * x; };

constexpr float square_distance(Point a, Point b) { return square(a.x - b.x) + square(a.y - b.y); }

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

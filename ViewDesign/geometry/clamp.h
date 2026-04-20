#pragma once

#include "ViewDesign/geometry/rect.h"


namespace ViewDesign {


constexpr float clamp(float pos, float pos_min, float pos_max) {
	if (pos < pos_min) { pos = pos_min; }
	if (pos > pos_max) { pos = pos_max; }
	return pos;
}

constexpr float clamp(float number, Interval range) {
	return clamp(number, range.left(), range.right());
}

constexpr Interval clamp(Interval interval, Interval range) {
	interval.length = clamp(interval.length, 0, range.length);
	interval.begin = clamp(interval.begin, range.begin, range.right() - interval.length);
	return interval;
}

constexpr Point clamp(Point point, Rect region) {
	return Point(clamp(point.x, Interval(region.point.x, region.size.width)), clamp(point.y, Interval(region.point.y, region.size.height)));
}

constexpr Rect clamp(Rect rect, Rect region) {
	return Rect(clamp(rect.Horizontal(), region.Horizontal()), clamp(rect.Vertical(), region.Vertical()));
}


} // namespace ViewDesign

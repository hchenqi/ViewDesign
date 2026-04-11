#pragma once

#include "geometry.h"


namespace ViewDesign {


constexpr Point ScalePointBySize(Point point, Size size) {
	return Point(point.x * size.width, point.y * size.height);
}

constexpr Size ScaleSizeBySize(Size old_size, Size size) {
	return Size(old_size.width * size.width, old_size.height * size.height);
}

constexpr Rect ScaleRectBySize(Rect region, Size size) {
	return Rect(ScalePointBySize(region.point, size), ScaleSizeBySize(region.size, size));
}


constexpr Rect RegionToOverlappingTileRange(Rect region, Size tile_size) {
	if (tile_size.IsEmpty()) { return region_empty; }
	Point point_begin(floorf(region.left() / tile_size.width), floorf(region.top() / tile_size.height));
	Point point_end(ceilf(region.right() / tile_size.width), ceilf(region.bottom() / tile_size.height));
	return Rect(point_begin, Size(point_end.x - point_begin.x, point_end.y - point_begin.y));
}


class RectPointIterator {
private:
	Point start, end, current;
public:
	RectPointIterator(Rect rect) : start(rect.point), end(rect.RightBottom()), current(start) {}
	operator bool() const { return current < end; }
	Point operator*() const { if (!*this) { throw std::logic_error("cannot dereference end iterator"); } return current; }
	void operator++() { current.x++; if (current.x >= end.x) { current.x = start.x; current.y++; } }
};


} // namespace ViewDesign

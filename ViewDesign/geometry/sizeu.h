#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/geometry/rect.h"

#include <cmath>


namespace ViewDesign {


struct PointI {
	int32 x;
	int32 y;

	explicit constexpr PointI(int32 x, int32 y) : x(x), y(y) {}

	constexpr bool operator==(const PointI& other) const { return x == other.x && y == other.y; }

	constexpr bool operator>(const PointI& other) const { return x > other.x && y > other.y; }
	constexpr bool operator<(const PointI& other) const { return x < other.x && y < other.y; }
	constexpr bool operator>=(const PointI& other) const { return x >= other.x && y >= other.y; }
	constexpr bool operator<=(const PointI& other) const { return x <= other.x && y <= other.y; }

	constexpr operator Point() const { return Point(x, y); }
};

struct SizeU {
	uint32 width;
	uint32 height;

	explicit constexpr SizeU(uint32 width, uint32 height) : width(width), height(height) {}

	constexpr bool IsEmpty() const { return width == 0 || height == 0; }

	constexpr bool operator==(const SizeU& other) const { return width == other.width && height == other.height; }

	constexpr operator Size() const { return Size(width, height); }
};

struct RectI {
	PointI point;
	SizeU size;

	explicit constexpr RectI(PointI point, SizeU size) : point(point), size(size) {}

	constexpr int32 left() const { return point.x; }
	constexpr int32 right() const { return point.x + size.width; }
	constexpr int32 top() const { return point.y; }
	constexpr int32 bottom() const { return point.y + size.height; }

	constexpr PointI LeftTop() const { return PointI(left(), top()); }
	constexpr PointI LeftBottom() const { return PointI(left(), bottom()); }
	constexpr PointI RightTop() const { return PointI(right(), top()); }
	constexpr PointI RightBottom() const { return PointI(right(), bottom()); }

	constexpr bool IsEmpty() const { return size.IsEmpty(); }

	constexpr RectI Union(const RectI& other) const {
		if (IsEmpty()) { return other; } if (other.IsEmpty()) { return *this; }
		PointI pl1 = LeftTop(), ph1 = RightBottom(), pl2 = other.LeftTop(), ph2 = other.RightBottom();
		PointI pl = PointI(std::min(pl1.x, pl2.x), std::min(pl1.y, pl2.y));
		PointI ph = PointI(std::max(ph1.x, ph2.x), std::max(ph1.y, ph2.y));
		return RectI(pl, SizeU(ph.x - pl.x, ph.y - pl.y));
	}

	constexpr RectI Intersect(const RectI& other) const {
		PointI pl1 = LeftTop(), ph1 = RightBottom(), pl2 = other.LeftTop(), ph2 = other.RightBottom();
		PointI pl = PointI(std::max(pl1.x, pl2.x), std::max(pl1.y, pl2.y));
		PointI ph = PointI(std::min(ph1.x, ph2.x), std::min(ph1.y, ph2.y));
		return ph > pl ? RectI(pl, SizeU(ph.x - pl.x, ph.y - pl.y)) : RectI(PointI(0, 0), SizeU(0, 0));
	}

	constexpr operator Rect() const { return Rect(point, size); }
};

constexpr PointI point_i_zero = PointI(0, 0);
constexpr SizeU size_u_empty = SizeU(0, 0);
constexpr RectI rect_i_empty = RectI(point_i_zero, size_u_empty);


inline PointI Round(Point point) {
	return PointI(roundf(point.x), roundf(point.y));
}

inline SizeU Round(Size size) {
	return SizeU(std::max(roundf(size.width), 0.0f), std::max(roundf(size.height), 0.0f));
}

inline SizeU RoundUp(Size size) {
	return SizeU(std::max(ceilf(size.width), 0.0f), std::max(ceilf(size.height), 0.0f));
}

inline RectI Round(Rect region) {
	float left = roundf(region.left()), top = roundf(region.top()), right = roundf(region.right()), bottom = roundf(region.bottom());
	return RectI(PointI(left, top), SizeU(std::max(right - left, 0.0f), std::max(bottom - top, 0.0f)));
}

inline RectI RoundUp(Rect region) {
	float left = floorf(region.left()), top = floorf(region.top()), right = ceilf(region.right()), bottom = ceilf(region.bottom());
	return RectI(PointI(left, top), SizeU(std::max(right - left, 0.0f), std::max(bottom - top, 0.0f)));
}


} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/point.h"
#include "ViewDesign/geometry/size.h"
#include "ViewDesign/geometry/interval.h"


namespace ViewDesign {


struct Rect {
	Point point;
	Size size;

	explicit constexpr Rect() = default;
	explicit constexpr Rect(Point point, Size size) : point(point), size(size) {}
	explicit constexpr Rect(Interval horizontal, Interval vertical) : point(horizontal.begin, vertical.begin), size(horizontal.length, vertical.length) {}
	explicit constexpr Rect(float x, float y, float width, float height) : point(x, y), size(width, height) {}

	constexpr bool operator==(const Rect& other) const { return point == other.point && size == other.size; }

	constexpr float left() const { return point.x; }
	constexpr float right() const { return point.x + size.width; }
	constexpr float top() const { return point.y; }
	constexpr float bottom() const { return point.y + size.height; }

	constexpr float width() const { return size.width; }
	constexpr float height() const { return size.height; }

	constexpr Point LeftTop() const { return Point(left(), top()); }
	constexpr Point LeftBottom() const { return Point(left(), bottom()); }
	constexpr Point RightTop() const { return Point(right(), top()); }
	constexpr Point RightBottom() const { return Point(right(), bottom()); }
	constexpr Point Center() const { return Point(point.x + size.width / 2, point.y + size.height / 2); }

	constexpr Interval Horizontal() const { return Interval(point.x, size.width); }
	constexpr Interval Vertical() const { return Interval(point.y, size.height); }

	constexpr bool IsEmpty() const { return size.IsEmpty(); }

	constexpr bool Contains(const Point& point) const { return point >= LeftTop() && point < RightBottom(); }
	constexpr bool Contains(const Rect& other) const { return other.LeftTop() >= LeftTop() && other.RightBottom() <= RightBottom(); }

	constexpr Rect Union(const Rect& other) const {
		if (IsEmpty()) { return other; } if (other.IsEmpty()) { return *this; }
		Point pl1 = LeftTop(), ph1 = RightBottom(), pl2 = other.LeftTop(), ph2 = other.RightBottom();
		Point pl = Point(std::min(pl1.x, pl2.x), std::min(pl1.y, pl2.y));
		Point ph = Point(std::max(ph1.x, ph2.x), std::max(ph1.y, ph2.y));
		return Rect(pl, Size(ph.x - pl.x, ph.y - pl.y));
	}

	constexpr Rect Intersect(const Rect& other) const {
		Point pl1 = LeftTop(), ph1 = RightBottom(), pl2 = other.LeftTop(), ph2 = other.RightBottom();
		Point pl = Point(std::max(pl1.x, pl2.x), std::max(pl1.y, pl2.y));
		Point ph = Point(std::min(ph1.x, ph2.x), std::min(ph1.y, ph2.y));
		return ph > pl ? Rect(pl, Size(ph.x - pl.x, ph.y - pl.y)) : Rect(Point(0.0f, 0.0f), Size(0.0f, 0.0f));
	}

	static Rect Union(const Rect& rect1, const Rect& rect2) { return rect1.Union(rect2); }
	static Rect Intersect(const Rect& rect1, const Rect& rect2) { return rect1.Intersect(rect2); }
};


} // namespace ViewDesign

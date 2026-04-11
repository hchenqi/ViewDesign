#pragma once

#include "point.h"
#include "size.h"
#include "interval.h"


namespace ViewDesign {


struct Rect {
	Point point;
	Size size;

	explicit constexpr Rect() : point(), size() {}
	explicit constexpr Rect(Point point, Size size) : point(point), size(size) {}
	explicit constexpr Rect(Interval horizontal, Interval vertical) : point(horizontal.begin, vertical.begin), size(horizontal.length, vertical.length) {}
	explicit constexpr Rect(float x, float y, float width, float height) : point(x, y), size(width, height) {}

	constexpr bool operator==(const Rect& rect) const { return point == rect.point && size == rect.size; }
	constexpr bool operator!=(const Rect& rect) const { return point != rect.point || size != rect.size; }

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

	constexpr bool Contains(const Point& point) const { return point >= this->point && point < RightBottom(); }
	constexpr bool Contains(const Rect& rect) const { return rect.point >= point && rect.RightBottom() <= RightBottom(); }

	constexpr Rect Intersect(const Rect& rect) const {
		Point posl1 = point, posl2 = rect.point, posh1 = RightBottom(), posh2 = rect.RightBottom();
		Point posl = Point(std::max(posl1.x, posl2.x), std::max(posl1.y, posl2.y));
		Point posh = Point(std::min(posh1.x, posh2.x), std::min(posh1.y, posh2.y));
		return posh > posl ? Rect(posl, Size(posh.x - posl.x, posh.y - posl.y)) : Rect();
	}

	constexpr Rect Union(const Rect& rect) const {
		if (IsEmpty()) { return rect; } if (rect.IsEmpty()) { return *this; }
		Point posl1 = point, posl2 = rect.point, posh1 = RightBottom(), posh2 = rect.RightBottom();
		Point posl = Point(std::min(posl1.x, posl2.x), std::min(posl1.y, posl2.y));
		Point posh = Point(std::max(posh1.x, posh2.x), std::max(posh1.y, posh2.y));
		return Rect(posl, Size(posh.x - posl.x, posh.y - posl.y));
	}

	static Rect Intersect(const Rect& rect1, const Rect& rect2) { return rect1.Intersect(rect2); }
	static Rect Union(const Rect& rect1, const Rect& rect2) { return rect1.Union(rect2); }
};


} // namespace ViewDesign

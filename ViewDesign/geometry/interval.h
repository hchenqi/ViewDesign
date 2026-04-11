#pragma once

#include "../common/core.h"


namespace ViewDesign {


struct Interval {
	float begin;
	float length;

	explicit constexpr Interval() : begin(), length() {}
	explicit constexpr Interval(float begin, float length) : begin(begin), length(length) {}

	constexpr bool operator==(const Interval& interval) const { return begin == interval.begin && length == interval.length; }
	constexpr bool operator!=(const Interval& interval) const { return begin != interval.begin || length != interval.length; }

	constexpr float left() const { return begin; }
	constexpr float right() const { return begin + length; }

	constexpr bool IsEmpty() const { return length == 0.0f; }

	constexpr bool Contains(float number) const { return number >= left() && number < right(); }
	constexpr bool Contains(const Interval& interval) const { return interval.left() >= left() && interval.right() <= right(); }

	constexpr Interval Intersect(const Interval& interval) const {
		float left_max = max(left(), interval.left()), right_min = min(right(), interval.right());
		return right_min > left_max ? Interval(left_max, right_min - left_max) : Interval();
	}

	constexpr Interval Union(const Interval& interval) const {
		if (IsEmpty()) { return interval; } if (interval.IsEmpty()) { return *this; }
		float left_min = min(left(), interval.left()), right_max = max(right(), interval.right());
		return Interval(left_min, right_max - left_min);
	}

	static Interval Intersect(const Interval& interval1, const Interval& interval2) { return interval1.Intersect(interval2); }
	static Interval Union(const Interval& interval1, const Interval& interval2) { return interval1.Union(interval2); }
};


constexpr Interval interval_empty;


} // namespace ViewDesign

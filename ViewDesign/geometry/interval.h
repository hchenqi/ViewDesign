#pragma once

#include <algorithm>


namespace ViewDesign {


struct Interval {
	float begin;
	float length;

	explicit constexpr Interval() = default;
	explicit constexpr Interval(float begin, float length) : begin(begin), length(length) {}

	constexpr bool operator==(const Interval& other) const { return begin == other.begin && length == other.length; }

	constexpr float left() const { return begin; }
	constexpr float right() const { return begin + length; }

	constexpr bool IsEmpty() const { return length == 0.0f; }

	constexpr bool Contains(float position) const { return position >= left() && position < right(); }
	constexpr bool Contains(const Interval& other) const { return other.left() >= left() && other.right() <= right(); }

	constexpr Interval Union(const Interval& other) const {
		if (IsEmpty()) { return other; } if (other.IsEmpty()) { return *this; }
		float left_min = std::min(left(), other.left()), right_max = std::max(right(), other.right());
		return Interval(left_min, right_max - left_min);
	}

	constexpr Interval Intersect(const Interval& other) const {
		float left_max = std::max(left(), other.left()), right_min = std::min(right(), other.right());
		return right_min > left_max ? Interval(left_max, right_min - left_max) : Interval(0.0f, 0.0f);
	}

	static Interval Union(const Interval& interval1, const Interval& interval2) { return interval1.Union(interval2); }
	static Interval Intersect(const Interval& interval1, const Interval& interval2) { return interval1.Intersect(interval2); }
};


} // namespace ViewDesign

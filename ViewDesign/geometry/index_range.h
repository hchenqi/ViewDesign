#pragma once

#include "../common/core.h"


namespace ViewDesign {


struct IndexRange {
	size_t _begin;
	size_t _length;

	explicit constexpr IndexRange() : _begin(), _length() {}
	explicit constexpr IndexRange(size_t begin, size_t length) : _begin(begin), _length(length) {}

	constexpr bool operator==(const IndexRange& range) const { return _begin == range._begin && _length == range._length; }
	constexpr bool operator!=(const IndexRange& range) const { return _begin != range._begin || _length != range._length; }

	constexpr size_t begin() const { return _begin; }
	constexpr size_t end() const { return _begin + _length; }

	constexpr size_t length() const { return _length; }
	constexpr bool empty() const { return _length == 0; }

	constexpr bool Contains(size_t position) const { return position >= begin() && position < end(); }
	constexpr bool Contains(const IndexRange& range) const { return range.begin() >= begin() && range.end() <= end(); }

	constexpr IndexRange Intersect(const IndexRange& range) const {
		size_t begin_max = std::max(begin(), range.begin()), end_min = std::min(end(), range.end());
		return end_min > begin_max ? IndexRange(begin_max, end_min - begin_max) : IndexRange();
	}

	constexpr IndexRange Union(const IndexRange& range) const {
		if (empty()) { return range; } if (range.empty()) { return *this; }
		size_t begin_min = std::min(begin(), range.begin()), end_max = std::max(end(), range.end());
		return IndexRange(begin_min, end_max - begin_min);
	}
};


constexpr IndexRange index_range_empty = IndexRange();


} // namespace ViewDesign

#pragma once

#include <algorithm>
#include <stdexcept>


namespace ViewDesign {


struct TileIndex {
	int x;
	int y;

	bool operator==(const TileIndex& other) const { return x == other.x && y == other.y; }
	bool operator>=(const TileIndex& other) const { return x >= other.x && y >= other.y; }
	bool operator<(const TileIndex& other) const { return x < other.x && y < other.y; }
};


struct TileRange {
	TileIndex _begin;
	TileIndex _end;

	struct Iterator {
		const TileRange& range;
		TileIndex current;

		bool OutOfRange() const { return !(current >= range._begin && current < range._end); }

		TileIndex operator*() const {
			if (OutOfRange()) { throw std::logic_error("TileRange::Iterator: out of range"); }
			return current;
		}
		bool operator==(const Iterator& other) const {
			if (&range != &other.range) { throw std::invalid_argument("TileRange::Iterator: range mismatch at comparision"); }
			if (OutOfRange() && other.OutOfRange()) { return true; }
			return current == other.current;
		}
		Iterator& operator++() {
			if (OutOfRange()) { throw std::logic_error("TileRange::Iterator: out of range"); }
			current.x++;
			if (current.x >= range._end.x) {
				current.x = range._begin.x;
				current.y++;
			}
			return *this;
		}
	};

	Iterator begin() const { return { *this, _begin }; }
	Iterator end() const { return { *this, _end }; }
};


} // namespace ViewDesign

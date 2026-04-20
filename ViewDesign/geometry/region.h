#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/geometry/geometry.h"

#include <vector>


namespace ViewDesign {


class Region : Uncopyable {
private:
	using HANDLE = void*;
	HANDLE rgn;

public:
	Region(Rect region = region_empty);
	~Region();

	bool IsEmpty() const;
	void Set(Rect region);
	void Clear() { Set(region_empty); }

	void Translate(Vector vector);

	void Union(const Rect& region);
	void Intersect(const Rect& region);
	void Sub(const Rect& region);
	void Xor(const Rect& region);

	void Union(const Region& region);
	void Intersect(const Region& region);
	void Sub(const Region& region);
	void Xor(const Region& region);

	Rect GetBoundingRect() const;
	std::pair<Rect, std::vector<Rect>> GetRects() const;
};


} // namespace ViewDesign

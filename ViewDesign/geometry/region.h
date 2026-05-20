#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/geometry/sizeu.h"

#include <vector>


namespace ViewDesign {


class Region : Uncopyable {
private:
	Handle handle;

public:
	Region(RectI region = rect_i_empty);
	~Region();

	bool IsEmpty() const;
	void Set(RectI region);
	void Clear() { Set(rect_i_empty); }

	void Union(const RectI& region);
	void Intersect(const RectI& region);
	void Sub(const RectI& region);
	void Xor(const RectI& region);

	void Union(const Region& region);
	void Intersect(const Region& region);
	void Sub(const Region& region);
	void Xor(const Region& region);

	RectI GetBoundingRect() const;
	std::pair<RectI, std::vector<RectI>> GetRects() const;
};


} // namespace ViewDesign

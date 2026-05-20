// Note: this implementation is incorrect and incomplete, it works currently just as a placeholder for LayerFrame to work with glfw.

#include "ViewDesign/geometry/region.h"


namespace ViewDesign {

namespace {

inline ref_ptr<RectI> AsRectI(Handle handle) { return static_cast<ref_ptr<RectI>>(handle); }

} // namespace


Region::Region(RectI region) { handle = new RectI(region); }
Region::~Region() { delete static_cast<owner_ptr<RectI>>(handle); }

bool Region::IsEmpty() const { return AsRectI(handle)->IsEmpty(); }

void Region::Set(RectI region) { *AsRectI(handle) = region; }

void Region::Union(const Region& region) { Union(*AsRectI(region.handle)); }
void Region::Intersect(const Region& region) { Intersect(*AsRectI(region.handle)); }
void Region::Sub(const Region& region) { Sub(*AsRectI(region.handle)); }
void Region::Xor(const Region& region) { Xor(*AsRectI(region.handle)); }

void Region::Union(const RectI& region) { *AsRectI(handle) = AsRectI(handle)->Union(region); }
void Region::Intersect(const RectI& region) { *AsRectI(handle) = AsRectI(handle)->Intersect(region); }
void Region::Sub(const RectI& region) { if (region.Contains(*AsRectI(handle))) { *AsRectI(handle) = rect_i_empty; } }
void Region::Xor(const RectI& region) { *AsRectI(handle) = rect_i_empty; }

RectI Region::GetBoundingRect() const { return *AsRectI(handle); }
std::pair<RectI, std::vector<RectI>> Region::GetRects() const { return { *AsRectI(handle), { *AsRectI(handle) } }; }


} // namespace ViewDesign

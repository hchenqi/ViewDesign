// Note: this implementation is incorrect and incomplete, it works currently just as a placeholder for LayerFrame to work with glfw.

#include "ViewDesign/geometry/region.h"


namespace ViewDesign {

namespace {

inline ref_ptr<Rect> AsRect(Handle handle) { return static_cast<ref_ptr<Rect>>(handle); }

} // namespace


Region::Region(Rect region) { handle = new Rect(region); }
Region::~Region() { delete static_cast<owner_ptr<Rect>>(handle); }

bool Region::IsEmpty() const { return AsRect(handle)->IsEmpty(); }

void Region::Set(Rect region) { *AsRect(handle) = region; }
void Region::Translate(Vector vector) { AsRect(handle)->point += vector; }

void Region::Union(const Region& region) { Union(*AsRect(region.handle)); }
void Region::Intersect(const Region& region) { Intersect(*AsRect(region.handle)); }
void Region::Sub(const Region& region) { Sub(*AsRect(region.handle)); }
void Region::Xor(const Region& region) { Xor(*AsRect(region.handle)); }

void Region::Union(const Rect& region) { *AsRect(handle) = AsRect(handle)->Union(region); }
void Region::Intersect(const Rect& region) { *AsRect(handle) = AsRect(handle)->Intersect(region); }
void Region::Sub(const Rect& region) { if (region.Contains(*AsRect(handle))) { *AsRect(handle) = rect_empty; } }
void Region::Xor(const Rect& region) { *AsRect(handle) = rect_empty; }

Rect Region::GetBoundingRect() const { return *AsRect(handle); }
std::pair<Rect, std::vector<Rect>> Region::GetRects() const { return { *AsRect(handle), { *AsRect(handle) } }; }


} // namespace ViewDesign

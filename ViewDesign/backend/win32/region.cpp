#include "ViewDesign/geometry/region.h"
#include "ViewDesign/geometry/helper.h"
#include "ViewDesign/platform/win32/geometry_helper.h"


namespace ViewDesign {

using namespace Win32;

namespace {

Region region_temp;

Region& TempRegion(Rect rect) { region_temp.Set(rect); return region_temp; }

} // namespace


Region::Region(Rect region) {
	RECT rect = AsWin32RECT(Round(region));
	handle = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
}

Region::~Region() {
	DeleteObject(handle);
}

bool Region::IsEmpty() const {
	return CombineRgn((HRGN)handle, (HRGN)handle, NULL, RGN_COPY) == NULLREGION;
}

void Region::Set(Rect region) {
	RECT rect = AsWin32RECT(Round(region));
	SetRectRgn((HRGN)handle, rect.left, rect.top, rect.right, rect.bottom);
}

void Region::Translate(Vector vector) {
	OffsetRgn((HRGN)handle, (int)roundf(vector.x), (int)roundf(vector.y));
}

void Region::Union(const Region& region) {
	CombineRgn((HRGN)handle, (HRGN)handle, (HRGN)region.handle, RGN_OR);
}

void Region::Intersect(const Region& region) {
	CombineRgn((HRGN)handle, (HRGN)handle, (HRGN)region.handle, RGN_AND);
}

void Region::Sub(const Region& region) {
	CombineRgn((HRGN)handle, (HRGN)handle, (HRGN)region.handle, RGN_DIFF);
}

void Region::Xor(const Region& region) {
	CombineRgn((HRGN)handle, (HRGN)handle, (HRGN)region.handle, RGN_XOR);
}

void Region::Union(const Rect& region) { Union(TempRegion(region)); }
void Region::Intersect(const Rect& region) { Intersect(TempRegion(region)); }
void Region::Sub(const Rect& region) { Sub(TempRegion(region)); }
void Region::Xor(const Rect& region) { Xor(TempRegion(region)); }

Rect Region::GetBoundingRect() const {
	RECT rect;
	GetRgnBox((HRGN)handle, &rect);
	return AsRectI(rect);
}

std::pair<Rect, std::vector<Rect>> Region::GetRects() const {
	static std::vector<char> buffer;
	int size = GetRegionData((HRGN)handle, 0, NULL); buffer.resize(size);
	GetRegionData((HRGN)handle, size, (LPRGNDATA)buffer.data());
	RGNDATA& data = *(LPRGNDATA)buffer.data();
	static_assert(sizeof(RECT) == sizeof(Rect));
	Rect bound = AsRectI(data.rdh.rcBound);
	std::vector<Rect> regions((Rect*)(buffer.data() + data.rdh.dwSize), (Rect*)(buffer.data() + data.rdh.dwSize) + data.rdh.nCount);
	for (auto& region : regions) { region = AsRectI(*reinterpret_cast<RECT*>(&region)); }
	return { bound , regions };
}


} // namespace ViewDesign

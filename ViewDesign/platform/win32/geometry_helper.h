#pragma once

#include "ViewDesign/geometry/geometry.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


inline RECT AsRECT(Rect rect) {
	return { (int)floorf(rect.left()), (int)floorf(rect.top()), (int)ceilf(rect.right()), (int)ceilf(rect.bottom()) };
}

inline Rect AsRect(RECT rect) {
	return Rect((float)rect.left, (float)rect.top, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
}


} // namespace Win32

} // namespace ViewDesign

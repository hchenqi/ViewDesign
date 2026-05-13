#pragma once

#include "ViewDesign/geometry/sizeu.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


inline RECT AsWin32RECT(RectI rect) {
	return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

inline RectI AsRectI(RECT rect) {
	return RectI(PointI(rect.left, rect.top), SizeU(rect.right - rect.left, rect.bottom - rect.top));
}


} // namespace Win32

} // namespace ViewDesign

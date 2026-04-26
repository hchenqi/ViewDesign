#pragma once

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


inline void SetWndStyle(HWND hwnd, int mask) {
	LONG style = GetWindowLongW(hwnd, GWL_EXSTYLE);
	if (!(style & mask)) {
		SetWindowLongW(hwnd, GWL_EXSTYLE, style | mask);
	}
}

inline void SetWndTopMost(HWND hwnd) {
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}


} // namespace Win32

} // namespace ViewDesign

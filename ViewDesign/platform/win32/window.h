#pragma once

#include "ViewDesign/common/type.h"

#include <windows.h>
#include <commctrl.h>


namespace ViewDesign {

namespace Win32 {


inline HWND AsHWND(Handle window) { return static_cast<HWND>(window); }


inline HINSTANCE GetHInstance() {
	static HINSTANCE hInstance = GetModuleHandleW(NULL);
	return hInstance;
}


inline void SetWndStyle(HWND hwnd, int add, int remove = 0) {
	LONG style = GetWindowLongW(hwnd, GWL_STYLE);
	LONG style_new = (style & ~remove) | add;
	if (style_new != style) {
		SetWindowLongW(hwnd, GWL_STYLE, style_new);
	}
}

inline void SetWndExStyle(HWND hwnd, int add, int remove = 0) {
	LONG style = GetWindowLongW(hwnd, GWL_EXSTYLE);
	LONG style_new = (style & ~remove) | add;
	if (style_new != style) {
		SetWindowLongW(hwnd, GWL_EXSTYLE, style_new);
	}
}

inline void SetWndTopMost(HWND hwnd) {
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}


inline LRESULT CALLBACK SubclassProcNoNC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (uMsg) {
	case WM_DESTROY: RemoveWindowSubclass(hWnd, SubclassProcNoNC, uIdSubclass); break;
	case WM_NCCALCSIZE: return 0;
	case WM_NCHITTEST: return HTCLIENT;
	case WM_NCPAINT: return 0;
	case WM_NCACTIVATE: return true;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
};

inline void SetWndUndecorated(HWND hwnd) {
	SetWndStyle(hwnd, WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_HSCROLL | WS_VSCROLL, WS_OVERLAPPEDWINDOW);
	SetWindowSubclass(hwnd, SubclassProcNoNC, 0, 0);
}


} // namespace Win32

} // namespace ViewDesign

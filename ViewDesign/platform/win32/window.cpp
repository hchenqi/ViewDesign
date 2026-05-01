#include "ViewDesign/platform/win32/window.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void SetWndStyle(Handle handle, int mask) {
	LONG style = GetWindowLongW((HWND)handle, GWL_EXSTYLE);
	if (!(style & mask)) {
		SetWindowLongW((HWND)handle, GWL_EXSTYLE, style | mask);
	}
}

void SetWndTopMost(Handle handle) {
	SetWindowPos((HWND)handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}


} // namespace Win32

} // namespace ViewDesign

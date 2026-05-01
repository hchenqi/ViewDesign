#include "ViewDesign/platform/win32/aero_snap.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void AeroSnapDraggingEffect(Handle handle) {
	SendMessageW((HWND)handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(Handle handle, BorderPosition border_position) {
	SendMessageW((HWND)handle, WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace Win32

} // namespace ViewDesign

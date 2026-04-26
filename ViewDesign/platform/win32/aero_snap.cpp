#include "ViewDesign/platform/win32/aero_snap.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void AeroSnapDraggingEffect(ViewBase& view) {
	SendMessageW((HWND)desktop.GetWindow(view).GetPlatformHandle(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(ViewBase& view, BorderPosition border_position) {
	SendMessageW((HWND)desktop.GetWindow(view).GetPlatformHandle(), WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace Win32

} // namespace ViewDesign

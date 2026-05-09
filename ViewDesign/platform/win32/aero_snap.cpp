#include "ViewDesign/platform/win32/aero_snap.h"
#include "ViewDesign/platform/win32/window.h"


namespace ViewDesign {

namespace Win32 {


void AeroSnapDraggingEffect(Handle window) {
	SendMessageW(AsHWND(window), WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(Handle window, BorderPosition border_position) {
	SendMessageW(AsHWND(window), WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace Win32

} // namespace ViewDesign

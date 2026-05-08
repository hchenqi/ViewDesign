#include "ViewDesign/platform/win32/aero_snap.h"
#include "ViewDesign/platform/win32/window.h"


namespace ViewDesign {

namespace Win32 {


void AeroSnapDraggingEffect(Handle handle) {
	SendMessageW(AsHWND(handle), WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(Handle handle, BorderPosition border_position) {
	SendMessageW(AsHWND(handle), WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace Win32

} // namespace ViewDesign

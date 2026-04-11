#include "win32_aero_snap.h"
#include "../window/global.h"
#include "../frame/DesktopFrame.h"

#include <Windows.h>


namespace ViewDesign {

struct DesktopFrameApi : DesktopFrame {
	DesktopFrame::hwnd;
};


void AeroSnapDraggingEffect(ViewBase& view) {
	HANDLE hwnd = static_cast<DesktopFrameApi&>(global.GetDesktopFrame(view)).hwnd;
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(ViewBase& view, BorderPosition border_position) {
	HANDLE hwnd = static_cast<DesktopFrameApi&>(global.GetDesktopFrame(view)).hwnd;
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace ViewDesign

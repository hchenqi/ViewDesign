#include "ViewDesign/platform/win32/aero_snap.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>


namespace ViewDesign {

struct WindowApi : Window {
	using Window::hwnd;
};


namespace Win32 {


void AeroSnapDraggingEffect(ViewBase& view) {
	HANDLE hwnd = static_cast<WindowApi&>(desktop.GetWindow(view)).hwnd;
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(ViewBase& view, BorderPosition border_position) {
	HANDLE hwnd = static_cast<WindowApi&>(desktop.GetWindow(view)).hwnd;
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace Win32

} // namespace ViewDesign

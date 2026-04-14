#include "win32_aero_snap.h"
#include "../view/global.h"
#include "../view/Window.h"

#include <Windows.h>


namespace ViewDesign {

struct WindowApi : Window {
	Window::hwnd;
};


void AeroSnapDraggingEffect(ViewBase& view) {
	HANDLE hwnd = static_cast<WindowApi&>(global.GetWindow(view)).hwnd;
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(ViewBase& view, BorderPosition border_position) {
	HANDLE hwnd = static_cast<WindowApi&>(global.GetWindow(view)).hwnd;
	SendMessageW((HWND)hwnd, WM_NCLBUTTONDOWN, (WPARAM)border_position, 0);
}


} // namespace ViewDesign

#include "ViewDesign/platform/win32/aero_snap.h"
#include "ViewDesign/platform/win32/window.h"

#include <array>
#include <stdexcept>


namespace ViewDesign {

namespace Win32 {

namespace {

std::array<WPARAM, static_cast<size_t>(BorderPosition::_Count)> border_position_mapping = {
	0,
	HTLEFT,
	HTTOP,
	HTRIGHT,
	HTBOTTOM,
	HTTOPLEFT,
	HTTOPRIGHT,
	HTBOTTOMLEFT,
	HTBOTTOMRIGHT,
};

} // namespace


void AeroSnapDraggingEffect(Handle window) {
	SendMessageW(AsHWND(window), WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void AeroSnapBorderResizingEffect(Handle window, BorderPosition border_position) {
	if (border_position >= BorderPosition::_Count) { throw std::invalid_argument("invalid border position"); }
	SendMessageW(AsHWND(window), WM_NCLBUTTONDOWN, border_position_mapping[static_cast<size_t>(border_position)], 0);
}


} // namespace Win32

} // namespace ViewDesign

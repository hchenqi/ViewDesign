#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/system/cursor.h"
#include "ViewDesign/common/reversion_wrapper.h"
#include "ViewDesign/platform/win32/win32_api.h"
#include "ViewDesign/platform/win32/ime.h"


namespace ViewDesign {


Size ViewDesign::Desktop::GetSize() const {
    return Win32::GetDesktopSize();
}

void Desktop::SetWindowCapture(Window& window) {
	Win32::SetCapture(window.hwnd);
}

void Desktop::ReleaseWindowCapture() {
	Win32::ReleaseCapture();
}

void Desktop::SetWindowFocus(ref_ptr<Window> window_focus) {
	Win32::SetFocus(window_focus? window_focus->hwnd : nullptr);
}

void Desktop::ImeWindowEnable() {
	Win32::ImeEnable(window_focus->hwnd);
}

void Desktop::ImeWindowDisable() {
	Win32::ImeDisable(window_focus->hwnd);
}

void Desktop::ImeWindowSetPosition(Window& window, Point point) {
	Win32::ImeSetPosition(window.hwnd, point);
}

void Desktop::EventLoop() { Win32::MessageLoop(); }

void Desktop::Terminate() { window_list.clear(); Win32::Terminate(); }


} // namespace ViewDesign

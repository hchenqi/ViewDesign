#pragma once

#include "ViewDesign/platform/win32/window.h"

#include <stdexcept>


namespace ViewDesign {

namespace Win32 {


class Context {
public:
	Context() {
		if (Initialized()) {
			throw std::logic_error("Win32: context already initialized");
		}
		helper_window = CreateWindowExW(
			0, L"STATIC", L"",
			WS_OVERLAPPEDWINDOW,
			0, 0, 1, 1,
			nullptr, nullptr, GetHInstance(), nullptr
		);
		if (helper_window == NULL) {
			throw std::runtime_error("Win32: create window error");
		}
	}
	~Context() {
		if (helper_window != nullptr) {
			DestroyWindow(helper_window); helper_window = nullptr;
		}
	}
protected:
	inline static HWND helper_window = nullptr;
public:
	static bool Initialized() { return helper_window != nullptr; }
	static HWND GetHelperWindow() { if (!Initialized()) { throw std::logic_error("Win32: context not initialized"); } return helper_window; }
};


} // namespace Win32

} // namespace ViewDesign

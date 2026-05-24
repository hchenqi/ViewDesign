#include "ViewDesign/system/window.h"
#include "ViewDesign/system/desktop.h"

#include <ViewDesign/platform/win32/string.h>
#include <ViewDesign/platform/win32/window.h>
#include <ViewDesign/platform/win32/geometry_helper.h>
#include <ViewDesign/platform/win32/ime.h>
#include "ViewDesign/view/Desktop.h"

#include <windows.h>
#include <windowsx.h>

#if defined(VIEWDESIGN_BACKEND_WIN32_OPENGL) || defined(VIEWDESIGN_BACKEND_WIN32_VULKAN)
#include <dwmapi.h>
#endif

#undef CreateWindow


namespace ViewDesign {

struct WindowPrivateAccess : Window {
	using Window::HasParent;
	using Window::GetRegion;
	using Window::SetScale;
	using Window::SetSize;
	using Window::SetPoint;
	using Window::State;
	using Window::SetState;
	using Window::OnDraw;
};

struct DesktopPrivateAccess : Desktop {
	using Desktop::window_list;
	using Desktop::GetWindowMinMaxRegion;
	using Desktop::LoseTrack;
	using Desktop::LoseCapture;
	using Desktop::LoseFocus;
	using Desktop::DispatchMouseEvent;
	using Desktop::DispatchKeyEvent;
};

using namespace Win32;


namespace {

inline ref_ptr<WindowPrivateAccess> GetWindow(HWND hwnd) {
	return reinterpret_cast<ref_ptr<WindowPrivateAccess>>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

inline DesktopPrivateAccess& GetDesktop() {
	return static_cast<DesktopPrivateAccess&>(Desktop::Get());
}

constexpr float dpi_default = 96.0f;

inline bool IsMouseMsg(UINT msg) { return WM_MOUSEFIRST <= msg && msg <= WM_MOUSELAST; }
inline bool IsKeyMsg(UINT msg) { return WM_KEYFIRST <= msg && msg <= WM_IME_KEYLAST; }

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static bool is_mouse_tracked = false;

	ref_ptr<WindowPrivateAccess> window = GetWindow(hwnd);
	if (window == nullptr) { goto WindowIrrelevantMessages; }

	// mouse message
	if (IsMouseMsg(msg)) {
		MouseEvent mouse_event;
		mouse_event.point = Point((float)GET_X_LPARAM(lparam), (float)GET_Y_LPARAM(lparam));
		mouse_event._key_state = (uint8)GET_KEYSTATE_WPARAM(wparam);;
		mouse_event.wheel_delta = GET_WHEEL_DELTA_WPARAM(wparam);
		switch (msg) {
		case WM_MOUSEMOVE: mouse_event.type = MouseEvent::Move;
			if (!is_mouse_tracked) {
				TRACKMOUSEEVENT track_mouse_event;
				track_mouse_event.cbSize = sizeof(TRACKMOUSEEVENT);
				track_mouse_event.dwFlags = TME_LEAVE;
				track_mouse_event.hwndTrack = hwnd;
				track_mouse_event.dwHoverTime = HOVER_DEFAULT;
				TrackMouseEvent(&track_mouse_event);
				is_mouse_tracked = true;
			}
			break;
		case WM_LBUTTONDOWN: mouse_event.type = MouseEvent::LeftDown; break;
		case WM_LBUTTONUP: mouse_event.type = MouseEvent::LeftUp; break;
		case WM_RBUTTONDOWN: mouse_event.type = MouseEvent::RightDown; break;
		case WM_RBUTTONUP: mouse_event.type = MouseEvent::RightUp; break;
		case WM_MBUTTONDOWN: mouse_event.type = MouseEvent::MiddleDown; break;
		case WM_MBUTTONUP: mouse_event.type = MouseEvent::MiddleUp; break;
		case WM_MOUSEWHEEL: mouse_event.type = MouseEvent::WheelVertical; mouse_event.point -= window->GetRegion().point - point_zero; break;
		case WM_MOUSEHWHEEL: mouse_event.type = MouseEvent::WheelHorizontal; mouse_event.point -= window->GetRegion().point - point_zero; break;
		default: return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		GetDesktop().DispatchMouseEvent(*window, mouse_event);
		return 0;
	}

	// keyboard message
	if (IsKeyMsg(msg)) {
		KeyEvent key_event;
		key_event.key = static_cast<Key::Value>(wparam);
		key_event.ch = { static_cast<u16char>(wparam), 0 };
		switch (msg) {
		case WM_KEYDOWN: key_event.type = KeyEvent::KeyDown; break;
		case WM_KEYUP: key_event.type = KeyEvent::KeyUp; break;
		case WM_CHAR: key_event.type = KeyEvent::Char; break;
		case WM_IME_STARTCOMPOSITION: key_event.type = KeyEvent::ImeBegin; break;
		case WM_IME_COMPOSITION: key_event.type = KeyEvent::ImeString; ImeUpdateString(hwnd, (uint32)lparam); break;
		case WM_IME_ENDCOMPOSITION: key_event.type = KeyEvent::ImeEnd; break;
		default: return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		GetDesktop().DispatchKeyEvent(key_event);
		return 0;
	}

	{
		switch (msg) {
			// layout
		case WM_GETMINMAXINFO: {
			auto [size_min, region_max] = GetDesktop().GetWindowMinMaxRegion(*window);
			MINMAXINFO* min_max_info = reinterpret_cast<MINMAXINFO*>(lparam);
			min_max_info->ptMaxPosition = { region_max.point.x, region_max.point.y };
			min_max_info->ptMaxSize = { (LONG)region_max.size.width, (LONG)region_max.size.height };
			min_max_info->ptMinTrackSize = { (LONG)size_min.width, (LONG)size_min.height };
			min_max_info->ptMaxTrackSize = min_max_info->ptMaxSize;
		} break;
		case WM_WINDOWPOSCHANGING: break;
		case WM_WINDOWPOSCHANGED: return DefWindowProc(hwnd, msg, wparam, lparam);
		case WM_MOVE: window->SetPoint(PointI((short)LOWORD(lparam), (short)HIWORD(lparam))); break;
		case WM_SIZE: window->SetState(static_cast<WindowPrivateAccess::State>(wparam <= 2 ? wparam : 2)); if (wparam != SIZE_MINIMIZED) { window->SetSize(SizeU(LOWORD(lparam), HIWORD(lparam))); } break;
		case WM_DPICHANGED: window->SetScale(LOWORD(wparam) / dpi_default); break;

			// drawing
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			window->OnDraw();
			EndPaint(hwnd, &ps);
		} break;
		case WM_ERASEBKGND: return true;

			// event
		case WM_MOUSELEAVE: is_mouse_tracked = false; GetDesktop().LoseTrack(); break;
		case WM_CAPTURECHANGED: GetDesktop().LoseCapture(); break;

			// convert scroll message to mouse wheel message
		case WM_HSCROLL:
		case WM_VSCROLL: {
			short wheel_delta = 0;
			short key_state = 0;
			Point cursor_position = GetCursorPosition();
			if (GetAsyncKeyState(VK_SHIFT)) { key_state |= MK_SHIFT; }
			if (GetAsyncKeyState(VK_CONTROL)) { key_state |= MK_CONTROL; }
			switch (LOWORD(wparam)) {
			case SB_LINEUP: case SB_PAGEUP: wheel_delta = WHEEL_DELTA; break;
			case SB_LINEDOWN: case SB_PAGEDOWN: wheel_delta = -WHEEL_DELTA; break;
			default: return 0;
			}
			return WndProc(
				hwnd, msg == WM_HSCROLL ? WM_MOUSEHWHEEL : WM_MOUSEWHEEL,
				(wheel_delta << 16) | key_state, ((short)cursor_position.y << 16) | (short)cursor_position.x
			);
		}

		case WM_CLOSE: GetDesktop().RemoveWindow(*window); break;

		default: goto WindowIrrelevantMessages;
		}
		return 0;
	}

WindowIrrelevantMessages:
	switch (msg) {
	case WM_CREATE: break;
	case WM_DESTROY: if (GetDesktop().window_list.empty()) { PostQuitMessage(0); } break;
	case WM_KILLFOCUS: GetDesktop().LoseFocus(); break;
	default: return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
	return 0;
}

const u16char wnd_class_name[] = u"ViewDesignWindow";
WNDCLASSEXW wnd_class = [] {
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.lpfnWndProc = DefWindowProcW;
	wcex.hInstance = GetHInstance();
	wcex.lpszClassName = as_wchar_str(wnd_class_name);
	return wcex;
}();

} // namespace


Handle CreateWindow(const u16string& title) {
	static ATOM wnd_class_atom = RegisterClassExW(&wnd_class);
	if (wnd_class_atom == 0) {
		throw std::runtime_error("Win32: register class error");
	}
	HWND hwnd = CreateWindowExW(
#if defined(VIEWDESIGN_BACKEND_WIN32_DIRECTX)
		WS_EX_NOREDIRECTIONBITMAP,
#endif
#if defined(VIEWDESIGN_BACKEND_WIN32_OPENGL) || defined(VIEWDESIGN_BACKEND_WIN32_VULKAN)
		NULL,
#endif
		as_wchar_str(wnd_class_name), as_wchar_str(title.c_str()),
		WS_OVERLAPPEDWINDOW,
		0, 0, 0, 0, NULL, NULL, GetHInstance(), NULL
	);
	if (hwnd == NULL) {
		throw std::runtime_error("Win32: create window error");
	}
#if defined(VIEWDESIGN_BACKEND_WIN32_OPENGL) || defined(VIEWDESIGN_BACKEND_WIN32_VULKAN)
	HRGN region = CreateRectRgn(0, 0, -1, -1);
	DWM_BLURBEHIND bb = {};
	bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	bb.hRgnBlur = region;
	bb.fEnable = TRUE;
	DwmEnableBlurBehindWindow(hwnd, &bb);
	DeleteObject(region);
#endif
	return hwnd;
}

void AttachWindow(Handle window, Window& view) {
	SetWindowLongPtrW(AsHWND(window), GWLP_USERDATA, (LONG_PTR)&view);
	SetWindowLongPtrW(AsHWND(window), GWLP_WNDPROC, (LONG_PTR)WndProc);
}

void DestroyWindow(Handle window) { DestroyWindow(AsHWND(window)); }

Scale GetWindowScale(Handle window) { return GetDpiForWindow(AsHWND(window)) / dpi_default; }

void SetWindowTitle(Handle window, const u16string& title) { SetWindowTextW(AsHWND(window), as_wchar_str(title.c_str())); }
void SetWindowRegion(Handle window, RectI region) { MoveWindow(AsHWND(window), region.point.x, region.point.y, region.size.width, region.size.height, false); }
void SetWindowOpacity(Handle window, float opacity) { SetWndExStyle(AsHWND(window), WS_EX_LAYERED); SetLayeredWindowAttributes(AsHWND(window), 0, opacity * 0xFF, LWA_ALPHA); }
void SetWindowCursor(Handle window, std::reference_wrapper<Cursor> cursor) { SetCursor(cursor); }

void ShowWindow(Handle window) { ShowWindow(AsHWND(window), SW_SHOWNOACTIVATE); }
void HideWindow(Handle window) { ShowWindow(AsHWND(window), SW_HIDE); }
void MinimizeWindow(Handle window) { ShowWindow(AsHWND(window), SW_MINIMIZE); }
void MaximizeWindow(Handle window) { ShowWindow(AsHWND(window), SW_MAXIMIZE); }
void RestoreWindow(Handle window) { ShowWindow(AsHWND(window), SW_RESTORE); }

void CloseWindow(Handle window) { SendMessageW(AsHWND(window), WM_CLOSE, 0, 0); }

void RedrawWindowRegion(Handle window, RectI region) { RECT rect = AsWin32RECT(region); InvalidateRect(AsHWND(window), &rect, false); }

void SetWindowCapture(Handle window) { SetCapture(AsHWND(window)); }
void ReleaseWindowCapture(Handle window) { ReleaseCapture(); }
void SetWindowFocus(Handle window) { SetFocus(AsHWND(window)); }

void ImeWindowEnable(Handle window) { ImeEnable(AsHWND(window)); }
void ImeWindowDisable(Handle window) { ImeDisable(AsHWND(window)); }
void ImeWindowSetPosition(Handle window, PointI point) { ImeSetPosition(AsHWND(window), point); }


} // namespace ViewDesign

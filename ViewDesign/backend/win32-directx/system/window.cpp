#include "ViewDesign/system/window.h"
#include "ViewDesign/system/desktop.h"
#include "ViewDesign/platform/win32/string.h"
#include "ViewDesign/platform/win32/window.h"
#include "ViewDesign/platform/win32/geometry_helper.h"
#include "ViewDesign/platform/win32/ime.h"
#include "ViewDesign/platform/win32/directx_resource.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>
#include <windowsx.h>

#undef CreateWindow


namespace ViewDesign {

struct WindowApi : Window {
	using Window::HasParent;
	using Window::GetRegion;
	using Window::SetScale;
	using Window::SetSize;
	using Window::SetPoint;
	using Window::GetMinMaxRegion;
	using Window::State;
	using Window::SetState;
	using Window::OnDraw;
};

struct DesktopApi : Desktop {
	using Desktop::RecreateWindowLayer;
	using Desktop::LoseTrack;
	using Desktop::LoseCapture;
	using Desktop::LoseFocus;
	using Desktop::DispatchMouseEvent;
	using Desktop::DispatchKeyEvent;
};

using namespace Win32;


namespace {

constexpr float dpi_default = 96.0f;

inline bool IsMouseMsg(UINT msg) { return WM_MOUSEFIRST <= msg && msg <= WM_MOUSELAST; }
inline bool IsKeyMsg(UINT msg) { return WM_KEYFIRST <= msg && msg <= WM_IME_KEYLAST; }

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static bool is_mouse_tracked = false;

	ref_ptr<WindowApi> window = reinterpret_cast<ref_ptr<WindowApi>>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (window == nullptr) { goto WindowIrrelevantMessages; }

	// mouse message
	if (IsMouseMsg(msg)) {
		MouseEvent mouse_event;
		mouse_event.point = Point((float)GET_X_LPARAM(lparam), (float)GET_Y_LPARAM(lparam));
		mouse_event._key_state = (uchar)GET_KEYSTATE_WPARAM(wparam);;
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
		static_cast<DesktopApi&>(desktop).DispatchMouseEvent(*window, mouse_event);
		return 0;
	}

	// keyboard message
	if (IsKeyMsg(msg)) {
		KeyEvent key_event;
		key_event.key = static_cast<Key>(wparam);
		key_event.ch = { static_cast<u16char>(wparam), 0 };
		switch (msg) {
		case WM_KEYDOWN: key_event.type = KeyEvent::KeyDown; break;
		case WM_KEYUP: key_event.type = KeyEvent::KeyUp; break;
		case WM_CHAR: key_event.type = KeyEvent::Char; break;
		case WM_IME_STARTCOMPOSITION: key_event.type = KeyEvent::ImeBegin; break;
		case WM_IME_COMPOSITION: key_event.type = KeyEvent::ImeString; ImeUpdateString(hwnd, (uint)lparam); break;
		case WM_IME_ENDCOMPOSITION: key_event.type = KeyEvent::ImeEnd; break;
		default: return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		static_cast<DesktopApi&>(desktop).DispatchKeyEvent(key_event);
		return 0;
	}

	{
		switch (msg) {
			// layout
		case WM_GETMINMAXINFO: {
			MINMAXINFO* min_max_info = reinterpret_cast<MINMAXINFO*>(lparam);
			auto [size_min, region_max] = window->GetMinMaxRegion(GetDesktopSize());
			min_max_info->ptMaxPosition = { (int)floorf(region_max.point.x), (int)floorf(region_max.point.y) };
			min_max_info->ptMaxSize = { (int)ceilf(region_max.size.width), (int)ceilf(region_max.size.height) };
			min_max_info->ptMinTrackSize = { (int)floorf(size_min.width), (int)floorf(size_min.height) };
			min_max_info->ptMaxTrackSize = min_max_info->ptMaxSize;
		}break;
		case WM_WINDOWPOSCHANGING: break;
		case WM_WINDOWPOSCHANGED: return DefWindowProc(hwnd, msg, wparam, lparam);
		case WM_MOVE: window->SetPoint(Point((short)LOWORD(lparam), (short)HIWORD(lparam))); break;
		case WM_SIZE: window->SetState(static_cast<WindowApi::State>(wparam <= 2 ? wparam : 2)); if (wparam != SIZE_MINIMIZED) { window->SetSize(Size(LOWORD(lparam), HIWORD(lparam))); } break;
		case WM_DPICHANGED: window->SetScale(LOWORD(wparam) / dpi_default); break;

			// drawing
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			try {
				window->OnDraw();
			} catch (std::runtime_error&) {
				DirectXRecreateResource();
				static_cast<DesktopApi&>(desktop).RecreateWindowLayer();
			}
			EndPaint(hwnd, &ps);
		}break;
		case WM_ERASEBKGND: return true;

			// event
		case WM_MOUSELEAVE: is_mouse_tracked = false; static_cast<DesktopApi&>(desktop).LoseTrack(); break;
		case WM_CAPTURECHANGED: static_cast<DesktopApi&>(desktop).LoseCapture(); break;

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

		case WM_CLOSE: static_cast<DesktopApi&>(desktop).RemoveWindow(*window); break;
		case WM_DESTROY: break;

		default: goto WindowIrrelevantMessages;
		}
		return 0;
	}

WindowIrrelevantMessages:
	switch (msg) {
	case WM_CREATE: break;
	case WM_KILLFOCUS: static_cast<DesktopApi&>(desktop).LoseFocus(); break;
	case WM_NCCALCSIZE: break;
	case WM_NCHITTEST: return HTCLIENT;
	case WM_NCPAINT: break;
	case WM_NCACTIVATE: return true;
	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

HINSTANCE hInstance = NULL;
const u16char wnd_class_name[] = u"ViewDesignWindow";
WNDCLASSEXW wnd_class = [] {
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance = GetModuleHandle(NULL);
	wcex.lpszClassName = as_wchar_str(wnd_class_name);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	return wcex;
}();

} // namespace


Handle CreateWindow(Window& window, const u16string& title) {
	static ATOM wnd_class_atom = RegisterClassExW(&wnd_class);
	if (wnd_class_atom == 0) {
		throw std::runtime_error("Win32: register class error");
	}
	HWND hwnd = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP, as_wchar_str(wnd_class_name), as_wchar_str(title.c_str()),
								WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_HSCROLL | WS_VSCROLL,
								0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) {
		throw std::runtime_error("Win32: create window error");
	}
	SetWindowLongPtrW((HWND)hwnd, GWLP_USERDATA, (LONG_PTR)&window);
	return hwnd;
}

void DestroyWindow(Handle handle) { DestroyWindow((HWND)handle); }

Scale GetWindowScale(Handle handle) { return GetDpiForWindow((HWND)handle) / dpi_default; }

void SetWindowTitle(Handle handle, const u16string& title) { SetWindowTextW((HWND)handle, as_wchar_str(title.c_str())); }
void SetWindowRegion(Handle handle, Rect region) { MoveWindow((HWND)handle, (int)floorf(region.point.x), (int)floorf(region.point.y), (int)ceilf(region.size.width), (int)ceilf(region.size.height), false); }
void SetWindowOpacity(Handle handle, uchar opacity) { SetWndStyle((HWND)handle, WS_EX_LAYERED);	SetLayeredWindowAttributes((HWND)handle, 0, opacity, LWA_ALPHA); }
void SetWindowCursor(Handle handle, std::reference_wrapper<Cursor> cursor) { SetCursor(cursor); }

void ShowWindow(Handle handle) { ShowWindow((HWND)handle, SW_SHOWNOACTIVATE); }
void HideWindow(Handle handle) { ShowWindow((HWND)handle, SW_HIDE); }
void MinimizeWindow(Handle handle) { ShowWindow((HWND)handle, SW_MINIMIZE); }
void MaximizeWindow(Handle handle) { ShowWindow((HWND)handle, SW_MAXIMIZE); }
void RestoreWindow(Handle handle) { ShowWindow((HWND)handle, SW_RESTORE); }

void CloseWindow(Handle handle) { SendMessageW((HWND)handle, WM_CLOSE, 0, 0); }

void RedrawWindowRegion(Handle handle, Rect region) { RECT rect = AsRECT(region); InvalidateRect((HWND)handle, &rect, false); }

void SetWindowCapture(Handle handle) { SetCapture((HWND)handle); }
void ReleaseWindowCapture(Handle handle) { ReleaseCapture(); }
void SetWindowFocus(Handle handle) { SetFocus((HWND)handle); }

void ImeWindowEnable(Handle handle) { ImeEnable((HWND)handle); }
void ImeWindowDisable(Handle handle) { ImeDisable((HWND)handle); }
void ImeWindowSetPosition(Handle handle, Point point) { ImeSetPosition((HWND)handle, point); }


} // namespace ViewDesign

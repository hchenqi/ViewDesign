#include "win32_api.h"
#include "win32_ime.h"
#include "../window/desktop.h"
#include "../frame/DesktopFrame.h"

#include <Windows.h>
#include <windowsx.h>


namespace ViewDesign {

struct DesktopFrameApi : DesktopFrame {
	DesktopFrame::GetRegion;
	DesktopFrame::SetScale;
	DesktopFrame::SetSize;
	DesktopFrame::SetPoint;
	DesktopFrame::GetMinMaxRegion;
	DesktopFrame::State;
	DesktopFrame::SetState;
	DesktopFrame::Destroy;
	DesktopFrame::OnDraw;
};

struct DesktopApi : Desktop {
	Desktop::LoseTrack;
	Desktop::LoseCapture;
	Desktop::LoseFocus;
	Desktop::DispatchMouseEvent;
	Desktop::DispatchKeyEvent;
};

namespace {

bool is_mouse_tracked = false;
DesktopApi& desktop = static_cast<DesktopApi&>(Desktop::Get());

constexpr float dpi_default = 96.0f;

inline bool IsMouseMsg(UINT msg) { return WM_MOUSEFIRST <= msg && msg <= WM_MOUSELAST; }
inline bool IsKeyboardMsg(UINT msg) { return WM_KEYFIRST <= msg && msg <= WM_IME_KEYLAST; }

inline RECT AsWin32Rect(Rect rect) { return { (int)floorf(rect.left()), (int)floorf(rect.top()), (int)ceilf(rect.right()), (int)ceilf(rect.bottom()) }; }
inline Rect AsRect(RECT rect) { return Rect((float)rect.left, (float)rect.top, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top)); }

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	ref_ptr<DesktopFrameApi> frame = reinterpret_cast<ref_ptr<DesktopFrameApi>>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (frame == nullptr) { goto FrameIrrelevantMessages; }

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
		case WM_MOUSEWHEEL: mouse_event.type = MouseEvent::WheelVertical; mouse_event.point -= frame->GetRegion().point - point_zero; break;
		case WM_MOUSEHWHEEL: mouse_event.type = MouseEvent::WheelHorizontal; mouse_event.point -= frame->GetRegion().point - point_zero; break;
		default: return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		desktop.DispatchMouseEvent(*frame, mouse_event);
		return 0;
	}

	// keyboard message
	if (IsKeyboardMsg(msg)) {
		KeyEvent key_event;
		key_event.key = static_cast<Key>(wparam);
		key_event.ch = static_cast<wchar>(wparam);
		switch (msg) {
		case WM_KEYDOWN: key_event.type = KeyEvent::KeyDown; break;
		case WM_KEYUP: key_event.type = KeyEvent::KeyUp; break;
		case WM_CHAR: key_event.type = KeyEvent::Char; break;
		case WM_IME_STARTCOMPOSITION: key_event.type = KeyEvent::ImeBegin; break;
		case WM_IME_COMPOSITION: key_event.type = KeyEvent::ImeString; ImeUpdateString(hwnd, (uint)lparam); break;
		case WM_IME_ENDCOMPOSITION: key_event.type = KeyEvent::ImeEnd; break;
		default: return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		desktop.DispatchKeyEvent(key_event);
		return 0;
	}

	{
		switch (msg) {
			// region message
		case WM_GETMINMAXINFO: {
			MINMAXINFO* min_max_info = reinterpret_cast<MINMAXINFO*>(lparam);
			auto [size_min, region_max] = frame->GetMinMaxRegion(Win32::GetDesktopSize());
			min_max_info->ptMaxPosition = { (int)floorf(region_max.point.x), (int)floorf(region_max.point.y) };
			min_max_info->ptMaxSize = { (int)ceilf(region_max.size.width), (int)ceilf(region_max.size.height) };
			min_max_info->ptMinTrackSize = { (int)floorf(size_min.width), (int)floorf(size_min.height) };
			min_max_info->ptMaxTrackSize = min_max_info->ptMaxSize;
		}break;
		case WM_WINDOWPOSCHANGING: break;
		case WM_WINDOWPOSCHANGED: return DefWindowProc(hwnd, msg, wparam, lparam);
		case WM_MOVE: frame->SetPoint(Point((short)LOWORD(lparam), (short)HIWORD(lparam))); break;
		case WM_SIZE: frame->SetState(static_cast<DesktopFrameApi::State>(wparam <= 2 ? wparam : 2));
			if (wparam != SIZE_MINIMIZED) { frame->SetSize(Size(LOWORD(lparam), HIWORD(lparam))); } break;

			// notifications
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			frame->OnDraw();
			EndPaint(hwnd, &ps);
		}break;
		case WM_ERASEBKGND: return true;
		case WM_MOUSELEAVE: is_mouse_tracked = false; desktop.LoseTrack(); break;
		case WM_CAPTURECHANGED: desktop.LoseCapture(); break;

		case WM_DPICHANGED: frame->SetScale(LOWORD(wparam) / dpi_default); break;

			// convert scroll message to mouse wheel message
		case WM_HSCROLL:
		case WM_VSCROLL: {
			short wheel_delta = 0;
			short key_state = 0;
			Point cursor_position = Win32::GetCursorPos();
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
		default: goto FrameIrrelevantMessages;
		}
		return 0;
	}

FrameIrrelevantMessages:
	switch (msg) {
	case WM_CREATE: break;
	case WM_DESTROY: if (frame != nullptr) { frame->Destroy(); }  break;

	case WM_KILLFOCUS: desktop.LoseFocus(); break;

	case WM_NCCALCSIZE: break;
	case WM_NCHITTEST: return HTCLIENT;
	case WM_NCPAINT: break;
	case WM_NCACTIVATE: return true;

	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

static const wchar_t wnd_class_name[] = L"ViewDesignFrame";
HINSTANCE hInstance = NULL;

inline void RegisterWndClass() {
	static bool is_wnd_class_registered = false;
	if (!is_wnd_class_registered) {
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance = GetModuleHandle(NULL);
		wcex.lpszClassName = wnd_class_name;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		ATOM res = RegisterClassExW(&wcex);
		if (res == 0) { throw std::runtime_error("register class error"); }
		is_wnd_class_registered = true;
	}
}

} // namespace

namespace Win32 {


Size GetDesktopSize() { RECT rect; SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0); return AsRect(rect).size; }

Point GetCursorPos() {
	POINT cursor_position; GetCursorPos(&cursor_position);
	return Point((float)cursor_position.x, (float)cursor_position.y);
}

HANDLE CreateWnd(Rect region, std::wstring title) {
	RegisterWndClass();
	HWND hwnd = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP, wnd_class_name, title.c_str(),
								WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_HSCROLL | WS_VSCROLL,
								(int)floorf(region.point.x), (int)floorf(region.point.y), (int)ceilf(region.size.width), (int)ceilf(region.size.height),
								NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) { throw std::runtime_error("create window error"); }
	return hwnd;
}

void DestroyWnd(HANDLE hwnd) { DestroyWindow((HWND)hwnd); }

void SetWndUserData(HANDLE hwnd, void* data) {
	SetWindowLongPtrW((HWND)hwnd, GWLP_USERDATA, (LONG_PTR)data);
}

float GetWndDpiScale(HANDLE hwnd) {
	return GetDpiForWindow((HWND)hwnd) / dpi_default;
}

Point GetCursorPosWithWndDpi(HANDLE hwnd) {
	Point point = GetCursorPos(); float dpi = GetWndDpiScale(hwnd);
	return Point(point.x / dpi, point.y / dpi);
}

void SetWndTitle(HANDLE hwnd, std::wstring title) {
	SetWindowTextW((HWND)hwnd, title.c_str());
}

void SetWndRegion(HANDLE hwnd, Rect region) {
	MoveWindow((HWND)hwnd, (int)floorf(region.point.x), (int)floorf(region.point.y), (int)ceilf(region.size.width), (int)ceilf(region.size.height), false);
}

void SetWndStyleTool(HANDLE hwnd, bool style_tool) {
	LONG style = GetWindowLong((HWND)hwnd, GWL_EXSTYLE);
	if (style_tool) {
		if (!(style & WS_EX_TOOLWINDOW)) {
			style |= WS_EX_TOOLWINDOW;
			SetWindowLong((HWND)hwnd, GWL_EXSTYLE, style);
		}
	} else {
		if ((style & WS_EX_TOOLWINDOW)) {
			style &= ~WS_EX_TOOLWINDOW;
			SetWindowLong((HWND)hwnd, GWL_EXSTYLE, style);
		}
	}
}

void SetWndTopMost(HANDLE hwnd, bool top_most) {
	SetWindowPos((HWND)hwnd, top_most ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void SetWndOpacity(HANDLE hwnd, uchar opacity) {
	LONG style = GetWindowLong((HWND)hwnd, GWL_EXSTYLE);
	if (!(style & WS_EX_LAYERED)) {
		style |= WS_EX_LAYERED;
		SetWindowLong((HWND)hwnd, GWL_EXSTYLE, style);
	}
	SetLayeredWindowAttributes((HWND)hwnd, 0, opacity, LWA_ALPHA);
}

void SetWndMousePenetrate(HANDLE hwnd, bool mouse_penetrate) {
	LONG style = GetWindowLong((HWND)hwnd, GWL_EXSTYLE);
	if (mouse_penetrate) {
		if (!(style & WS_EX_TRANSPARENT)) {
			style |= WS_EX_TRANSPARENT;
			SetWindowLong((HWND)hwnd, GWL_EXSTYLE, style);
		}
	} else {
		if ((style & WS_EX_TRANSPARENT)) {
			style &= ~WS_EX_TRANSPARENT;
			SetWindowLong((HWND)hwnd, GWL_EXSTYLE, style);
		}
	}
}

void ShowWnd(HANDLE hwnd) { ShowWindow((HWND)hwnd, SW_SHOWDEFAULT); }
void HideWnd(HANDLE hwnd) { ShowWindow((HWND)hwnd, SW_HIDE); }
void SetForegroundWnd(HANDLE hwnd) { SetForegroundWindow((HWND)hwnd); }
void MinimizeWnd(HANDLE hwnd) { ShowWindow((HWND)hwnd, SW_MINIMIZE); }
void MaximizeWnd(HANDLE hwnd) { ShowWindow((HWND)hwnd, SW_MAXIMIZE); }
void RestoreWnd(HANDLE hwnd) { ShowWindow((HWND)hwnd, SW_RESTORE); }

void InvalidateWndRegion(HANDLE hwnd, Rect region) {
	RECT rect = AsWin32Rect(region);
	InvalidateRect((HWND)hwnd, &rect, false);
}

void SetCapture(HANDLE hwnd) { ::SetCapture((HWND)hwnd); }
void ReleaseCapture() { ::ReleaseCapture(); }
void SetFocus(HANDLE hwnd) { ::SetFocus((HWND)hwnd); }

int MessageLoop() {
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return (int)msg.wParam;
}

bool CheckMessage() {
	MSG msg;
	return PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
}

void Terminate() { PostQuitMessage(0); }


} // namespace Win32

} // namespace ViewDesign

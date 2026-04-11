#pragma once

#include "../geometry/geometry.h"

#include <string>


namespace ViewDesign {

using HANDLE = void*;


namespace Win32 {


Size GetDesktopSize();
Point GetCursorPos();

HANDLE CreateWnd(Rect region, std::wstring title);
void DestroyWnd(HANDLE hwnd);
void SetWndUserData(HANDLE hwnd, void* data);

float GetWndDpiScale(HANDLE hwnd);
Point GetCursorPosWithWndDpi(HANDLE hwnd);

void SetWndTitle(HANDLE hwnd, std::wstring title);
void SetWndRegion(HANDLE hwnd, Rect region);
void SetWndStyleTool(HANDLE hwnd, bool style_tool = true);
void SetWndTopMost(HANDLE hwnd, bool top_most = true);
void SetWndOpacity(HANDLE hwnd, uchar opacity);
void SetWndMousePenetrate(HANDLE hwnd, bool mouse_penetrate = true);

void ShowWnd(HANDLE hwnd);
void HideWnd(HANDLE hwnd);
void SetForegroundWnd(HANDLE hwnd);
void MinimizeWnd(HANDLE hwnd);
void MaximizeWnd(HANDLE hwnd);
void RestoreWnd(HANDLE hwnd);

void InvalidateWndRegion(HANDLE hwnd, Rect region);

void SetCapture(HANDLE hwnd);
void ReleaseCapture();
void SetFocus(HANDLE hwnd);

int MessageLoop();
bool CheckMessage();
void Terminate();


} // namespace Win32

} // namespace ViewDesign

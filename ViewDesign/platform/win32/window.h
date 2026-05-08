#pragma once

#include <windows.h>


namespace ViewDesign {

using Handle = void*;


namespace Win32 {


inline HWND AsHWND(Handle handle) { return static_cast<HWND>(handle); }


void SetWndStyle(HWND hwnd, int mask);

void SetWndTopMost(HWND hwnd);


} // namespace Win32

} // namespace ViewDesign

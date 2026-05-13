#pragma once

#include "ViewDesign/common/unicode.h"
#include "ViewDesign/geometry/sizeu.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void ImeEnable(HWND hwnd);
void ImeDisable(HWND hwnd);
void ImeSetPosition(HWND hwnd, PointI point);
void ImeUpdateString(HWND hwnd, uint32 type);

u16string ImeGetString();
size_t ImeGetCursorPosition();


} // namespace Win32

} // namespace ViewDesign

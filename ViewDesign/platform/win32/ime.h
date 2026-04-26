#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/geometry/point.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


void ImeEnable(HWND hwnd);
void ImeDisable(HWND hwnd);
void ImeSetPosition(HWND hwnd, Point point);
void ImeUpdateString(HWND hwnd, uint type);

u16string ImeGetString();
size_t ImeGetCursorPosition();


} // namespace Win32

} // namespace ViewDesign

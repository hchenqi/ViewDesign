#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/geometry/point.h"


namespace ViewDesign {

namespace Win32 {

using HANDLE = void*;


void ImeEnable(HANDLE hwnd);
void ImeDisable(HANDLE hwnd);
void ImeSetPosition(HANDLE hwnd, Point point);
void ImeUpdateString(HANDLE hwnd, uint type);

u16string ImeGetString();
size_t ImeGetCursorPosition();


} // namespace Win32

} // namespace ViewDesign

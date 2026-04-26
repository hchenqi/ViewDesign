#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/geometry/geometry.h"


namespace ViewDesign {

class Window;

using Handle = void*;


Handle CreateWindow(Window& window, const u16string& title);
void DestroyWindow(Handle handle);

Scale GetWindowScale(Handle handle);

void SetWindowTitle(Handle handle, const u16string& title);
void SetWindowRegion(Handle handle, Rect region);
void SetWindowOpacity(Handle handle, uchar opacity);

void ShowWindow(Handle handle);
void HideWindow(Handle handle);
void MinimizeWindow(Handle handle);
void MaximizeWindow(Handle handle);
void RestoreWindow(Handle handle);
void CloseWindow(Handle handle);

void RedrawWindowRegion(Handle handle, Rect region);

void SetWindowCapture(Handle handle);
void ReleaseWindowCapture();
void SetWindowFocus(Handle handle);

void ImeWindowEnable(Handle handle);
void ImeWindowDisable(Handle handle);
void ImeWindowSetPosition(Handle handle, Point point);


} // namespace ViewDesign

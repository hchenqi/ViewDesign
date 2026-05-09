#pragma once

#include "ViewDesign/common/unicode.h"
#include "ViewDesign/system/cursor.h"


namespace ViewDesign {

class Window;

using Handle = void*;


Handle CreateWindow(Window& window, const u16string& title);
void DestroyWindow(Handle window);

Scale GetWindowScale(Handle window);

void SetWindowTitle(Handle window, const u16string& title);
void SetWindowRegion(Handle window, Rect region);
void SetWindowOpacity(Handle window, uchar opacity);
void SetWindowCursor(Handle window, std::reference_wrapper<Cursor> cursor);

void ShowWindow(Handle window);
void HideWindow(Handle window);
void MinimizeWindow(Handle window);
void MaximizeWindow(Handle window);
void RestoreWindow(Handle window);
void CloseWindow(Handle window);

void RedrawWindowRegion(Handle window, Rect region);

void SetWindowCapture(Handle window);
void ReleaseWindowCapture(Handle window);
void SetWindowFocus(Handle window);

void ImeWindowEnable(Handle window);
void ImeWindowDisable(Handle window);
void ImeWindowSetPosition(Handle window, Point point);


} // namespace ViewDesign

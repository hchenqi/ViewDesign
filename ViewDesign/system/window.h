#pragma once

#include "ViewDesign/common/unicode.h"
#include "ViewDesign/system/cursor.h"


namespace ViewDesign {

class Window;


Handle CreateWindow(Window& window, const u16string& title);
void DestroyWindow(Handle window);

Scale GetWindowScale(Handle window);

void SetWindowTitle(Handle window, const u16string& title);
void SetWindowRegion(Handle window, RectI region);
void SetWindowOpacity(Handle window, float opacity);
void SetWindowCursor(Handle window, std::reference_wrapper<Cursor> cursor);

void ShowWindow(Handle window);
void HideWindow(Handle window);
void MinimizeWindow(Handle window);
void MaximizeWindow(Handle window);
void RestoreWindow(Handle window);
void CloseWindow(Handle window);

void RedrawWindowRegion(Handle window, RectI region);

void SetWindowCapture(Handle window);
void ReleaseWindowCapture(Handle window);
void SetWindowFocus(Handle window);

void ImeWindowEnable(Handle window);
void ImeWindowDisable(Handle window);
void ImeWindowSetPosition(Handle window, PointI point);


} // namespace ViewDesign

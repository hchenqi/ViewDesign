#pragma once

#include "ViewDesign/style/cursor_style.h"
#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/geometry/geometry.h"

#include <functional>


namespace ViewDesign {

class Cursor;


std::reference_wrapper<Cursor> GetCursor(CursorStyle style);
std::reference_wrapper<Cursor> CreateCursor(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot);


#if defined(VIEWDESIGN_BACKEND_WIN32)
void SetCursor(std::reference_wrapper<Cursor> cursor);
#endif

Point GetCursorPosition();


} // namespace ViewDesign

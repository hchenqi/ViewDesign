#pragma once

#include "ViewDesign/style/cursor_style.h"
#include "viewDesign/drawing/pixel_buffer.h"

#include <functional>


namespace ViewDesign {

class Cursor;


std::reference_wrapper<Cursor> GetCursor(CursorStyle style);
std::reference_wrapper<Cursor> CreateCursor(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot);

void SetCursor(std::reference_wrapper<Cursor> cursor);


} // namespace ViewDesign

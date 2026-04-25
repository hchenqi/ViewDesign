#pragma once

#include "ViewDesign/system/cursor.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


HCURSOR CreateCursorFromPixelBuffer(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot);


} // namespace Win32

} // namespace ViewDesign

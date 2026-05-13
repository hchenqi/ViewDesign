#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"

#include <windows.h>


namespace ViewDesign {

namespace Win32 {


HCURSOR CreateCursorFromPixelBuffer(const PixelBuffer& pixel_buffer, PointI hotspot);


} // namespace Win32

} // namespace ViewDesign

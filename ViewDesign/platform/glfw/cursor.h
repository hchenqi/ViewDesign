#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {

namespace GLFW {


owner_ptr<GLFWcursor> CreateCursorFromPixelBuffer(const PixelBuffer& pixel_buffer, PointI hotspot);


} // namespace GLFW

} // namespace ViewDesign

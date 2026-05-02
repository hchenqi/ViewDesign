#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {

namespace GLFW {


owner_ptr<GLFWcursor> CreateCursorFromPixelBuffer(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot);


} // namespace GLFW

} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/sizeu.h"

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {

namespace Vulkan {


inline vk::Offset2D AsVulkanOffset2D(PointI point) { return { point.x, point.y }; }
inline vk::Extent2D AsVulkanExtent2D(SizeU size) { return { size.width, size.height }; }
inline vk::Rect2D AsVulkanRect2D(RectI rect) { return { AsVulkanOffset2D(rect.point), AsVulkanExtent2D(rect.size) }; }


} // namespace Vulkan

} // namespace ViewDesign

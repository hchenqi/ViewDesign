#pragma once

#include "ViewDesign/geometry/sizeu.h"

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {

namespace Vulkan {


constexpr vk::Offset2D AsVulkanOffset2D(PointI point) { return { point.x, point.y }; }
constexpr vk::Extent2D AsVulkanExtent2D(SizeU size) { return { size.width, size.height }; }
constexpr vk::Rect2D AsVulkanRect2D(RectI rect) { return { AsVulkanOffset2D(rect.point), AsVulkanExtent2D(rect.size) }; }

constexpr PointI AsPointI(vk::Offset2D offset) { return PointI(offset.x, offset.y); }
constexpr SizeU AsSizeU(vk::Extent2D extent) { return SizeU(extent.width, extent.height); }
constexpr RectI AsRectI(vk::Rect2D rect) { return RectI(AsPointI(rect.offset), AsSizeU(rect.extent)); }


} // namespace Vulkan

} // namespace ViewDesign

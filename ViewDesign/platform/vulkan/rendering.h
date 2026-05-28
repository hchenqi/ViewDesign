#pragma once

#include "ViewDesign/platform/vulkan/image.h"


namespace ViewDesign {

namespace Vulkan {


inline void Render(vk::raii::CommandBuffer& command_buffer, vk::Image image, vk::ImageLayout& image_layout, vk::raii::ImageView& image_view, vk::ImageLayout image_layout_final, vk::Extent2D extent, vk::Rect2D clip_region, auto func) {
	static constexpr vk::ClearValue clear_value = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }));

	TransitionImageLayout(command_buffer, image, image_layout, vk::ImageLayout::eColorAttachmentOptimal);
	vk::RenderingAttachmentInfo attachment_info(*image_view, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clear_value);
	command_buffer.beginRendering(vk::RenderingInfo({}, clip_region, 1, 0, attachment_info));

	func();

	command_buffer.endRendering();
	TransitionImageLayout(command_buffer, image, image_layout, image_layout_final);
}


} // namespace Vulkan

} // namespace ViewDesign

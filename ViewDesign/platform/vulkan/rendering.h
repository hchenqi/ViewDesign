#pragma once

#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

namespace Vulkan {


inline void Render(vk::Image image, vk::ImageLayout& image_layout, vk::raii::ImageView& image_view, vk::ImageLayout image_layout_final, vk::Extent2D extent, RectI clip_region, auto func) {
	static constexpr vk::ClearValue clear_value = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }));

	FrameInFlight& frame_in_flight = FrameInFlight::GetCurrent();
	vk::raii::CommandBuffer& command_buffer = frame_in_flight.command_buffer;

	TransitionImageLayout(command_buffer, image, image_layout, vk::ImageLayout::eColorAttachmentOptimal);
	vk::RenderingAttachmentInfo attachment_info(*image_view, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clear_value);
	command_buffer.beginRendering(vk::RenderingInfo({}, AsVulkanRect2D(clip_region), 1, 0, attachment_info));

	RenderContext context(frame_in_flight, extent);
	func(static_cast<RenderTarget&>(context));

	command_buffer.endRendering();
	TransitionImageLayout(command_buffer, image, image_layout, image_layout_final);
}


} // namespace Vulkan

} // namespace ViewDesign

#pragma once

#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

namespace Vulkan {


inline void Render(vk::RenderPass render_pass, vk::raii::Framebuffer& framebuffer, vk::Extent2D extent, Rect clip_region, auto func) {
	static constexpr vk::ClearValue clear_value = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }));

	FrameInFlight& frame_in_flight = FrameInFlight::GetCurrent();
	frame_in_flight.command_buffer.beginRenderPass(vk::RenderPassBeginInfo(render_pass, framebuffer, AsVulkanRect2D(RoundUp(clip_region)), clear_value), vk::SubpassContents::eInline);

	RenderContext context(frame_in_flight, render_pass, extent);
	func(static_cast<RenderTarget&>(context));

	frame_in_flight.command_buffer.endRenderPass();
}


} // namespace Vulkan

} // namespace ViewDesign

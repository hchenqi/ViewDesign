#pragma once

#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

namespace Vulkan {


inline void Render(vk::raii::CommandBuffer& command_buffer, const vk::RenderPass& render_pass, vk::raii::Framebuffer& framebuffer, Rect clip_region, auto func) {
	static constexpr vk::ClearValue clear_value = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }));

	command_buffer.beginRenderPass(vk::RenderPassBeginInfo(render_pass, framebuffer, AsVulkanRect2D(RoundUp(clip_region)), clear_value), vk::SubpassContents::eInline);

	RenderTarget render_target(std::move(command_buffer));
	func(render_target);
	command_buffer = std::move(render_target);

	command_buffer.endRenderPass();
}


} // namespace Vulkan

} // namespace ViewDesign

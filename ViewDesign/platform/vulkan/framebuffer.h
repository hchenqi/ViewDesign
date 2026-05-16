#pragma once

#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/vulkan/texture.h"
#include "ViewDesign/platform/vulkan/render_pass.h"


namespace ViewDesign {

namespace Vulkan {


class Framebuffer : public Texture {
private:
	static constexpr vk::ImageLayout image_layout_final = vk::ImageLayout::eShaderReadOnlyOptimal;

protected:
	vk::raii::RenderPass  render_pass = nullptr;
	vk::raii::Framebuffer framebuffer = nullptr;

public:
	Framebuffer(SizeU size) : Texture(size) {
		DeviceContext& device_context = DeviceContext::Get();
		vk::raii::Device& device = device_context.device;

		vk::AttachmentDescription attachment({}, format, sample_count_flag_bits, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, image_layout, image_layout_final);
		vk::AttachmentReference attachment_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, attachment_reference);
		std::array<vk::SubpassDependency, 2> subpass_dependency_list = {
			vk::SubpassDependency(vk::SubpassExternal, 0, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite, vk::DependencyFlagBits::eByRegion),
			vk::SubpassDependency(0, vk::SubpassExternal, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead, vk::DependencyFlagBits::eByRegion),
		};
		render_pass = device.createRenderPass(vk::RenderPassCreateInfo({}, attachment, subpass, subpass_dependency_list));
		framebuffer = device.createFramebuffer(vk::FramebufferCreateInfo({}, render_pass, *image_view, extent.width, extent.height, 1));
	}

public:
	void Render(Rect clip_region, auto func) {
		DeviceContext& device_context = DeviceContext::Get();
		vk::raii::CommandBuffer& command_buffer = device_context.GetCurrentCommandBuffer();
		Vulkan::Render(command_buffer, render_pass, framebuffer, extent, clip_region, std::forward<decltype(func)>(func));
		image_layout = image_layout_final;
	}
};


} // namespace Vulkan

} // namespace ViewDesign

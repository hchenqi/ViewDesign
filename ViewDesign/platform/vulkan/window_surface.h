#pragma once

#include "ViewDesign/geometry/size.h"
#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


class WindowSurface {
private:
	vk::raii::SurfaceKHR               surface;
	vk::Extent2D                       extent;
	vk::raii::SwapchainKHR             swapchain = nullptr;
	std::vector<vk::raii::ImageView>   image_view_list;

	vk::raii::RenderPass               render_pass = nullptr;
	std::vector<vk::raii::Framebuffer> framebuffer_list;
	vk::raii::CommandPool              command_pool = nullptr;
	vk::raii::CommandBuffers           command_buffer_list = nullptr;

	std::vector<vk::raii::Semaphore>   image_available_semaphore_list;
	std::vector<vk::raii::Semaphore>   render_finished_semaphore_list;
	std::vector<vk::raii::Fence>       in_flight_fence_list;
	uint32_t                           current_frame_index = 0;

private:
	inline static vk::ClearValue       clear_value = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }));

public:
	WindowSurface(vk::raii::SurfaceKHR surface, Size size) : surface(std::move(surface)) { Initialize(size); }

private:
	inline static vk::SurfaceFormatKHR SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_format_list) {
		for (const auto& surface_format : surface_format_list) {
			if (surface_format.format == vk::Format::eB8G8R8A8Srgb && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return surface_format;
			}
		}
		return surface_format_list[0];
	}

	inline static vk::PresentModeKHR SelectPresentMode(const std::vector<vk::PresentModeKHR>& present_mode_list) {
		for (const auto& present_mode : present_mode_list) {
			if (present_mode == vk::PresentModeKHR::eMailbox) {
				return present_mode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	inline static vk::Extent2D ClampSizeToExtent(const vk::SurfaceCapabilitiesKHR& surface_capabilities, Size size) {
		return vk::Extent2D(
			std::clamp((uint32_t)size.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width),
			std::clamp((uint32_t)size.height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height)
		);
	}

private:
	void Initialize(Size size) {
		DeviceContext& device_context = DeviceContext::Get(surface);
		vk::raii::PhysicalDevice& physical_device = device_context.physical_device;
		vk::raii::Device& device = device_context.device;

		vk::SurfaceCapabilitiesKHR surface_capabilities = physical_device.getSurfaceCapabilitiesKHR(surface);
		vk::SurfaceFormatKHR surface_format = SelectSurfaceFormat(physical_device.getSurfaceFormatsKHR(surface));
		vk::PresentModeKHR present_mode = SelectPresentMode(physical_device.getSurfacePresentModesKHR(surface));

		extent = surface_capabilities.currentExtent == vk::Extent2D(UINT32_MAX, UINT32_MAX) ? ClampSizeToExtent(surface_capabilities, size) : surface_capabilities.currentExtent;

		swapchain = device.createSwapchainKHR(vk::SwapchainCreateInfoKHR(
			{}, surface, 2, surface_format.format, surface_format.colorSpace, extent, 1,
			vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
			{}, surface_capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
			present_mode, VK_TRUE
		));

		for (auto image : swapchain.getImages()) {
			image_view_list.emplace_back(device.createImageView(vk::ImageViewCreateInfo(
				{}, image, vk::ImageViewType::e2D, surface_format.format,
				{}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
			)));
		}

		vk::AttachmentDescription color_attachment({}, surface_format.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		vk::AttachmentReference color_attachment_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, color_attachment_reference);
		vk::SubpassDependency subpass_dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);
		render_pass = device.createRenderPass(vk::RenderPassCreateInfo({}, color_attachment, subpass, subpass_dependency));

		for (const auto& image_view : image_view_list) {
			framebuffer_list.emplace_back(device.createFramebuffer(vk::FramebufferCreateInfo({}, render_pass, *image_view, extent.width, extent.height, 1)));
		}

		command_pool = device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, device_context.queue_family_index));
		command_buffer_list = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(command_pool, vk::CommandBufferLevel::ePrimary, framebuffer_list.size()));

		for (uint32_t i = 0; i < framebuffer_list.size(); ++i) {
			image_available_semaphore_list.emplace_back(device.createSemaphore({}));
			render_finished_semaphore_list.emplace_back(device.createSemaphore({}));
			in_flight_fence_list.emplace_back(device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)));
		}
	}

public:
	void DrawFrame(auto f) {
		DeviceContext& device_context = DeviceContext::Get();

		auto wait_result = device_context.device.waitForFences(*in_flight_fence_list[current_frame_index], VK_TRUE, UINT64_MAX);
		device_context.device.resetFences(*in_flight_fence_list[current_frame_index]);

		auto [acquire_result, image_index] = swapchain.acquireNextImage(UINT64_MAX, *image_available_semaphore_list[current_frame_index]);
		if (acquire_result == vk::Result::eErrorOutOfDateKHR) {
			// recreate
		}

		vk::raii::CommandBuffer& command_buffer = command_buffer_list[current_frame_index];
		command_buffer.reset();
		command_buffer.begin(vk::CommandBufferBeginInfo{});
		command_buffer.beginRenderPass(vk::RenderPassBeginInfo(render_pass, framebuffer_list[image_index], vk::Rect2D({ 0, 0 }, extent), clear_value), vk::SubpassContents::eInline);

		f();

		command_buffer.endRenderPass();
		command_buffer.end();

		vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		device_context.queue.submit(vk::SubmitInfo(*image_available_semaphore_list[current_frame_index], wait_stage, *command_buffer_list[current_frame_index], *render_finished_semaphore_list[current_frame_index]), in_flight_fence_list[current_frame_index]);
		auto present_result = device_context.queue.presentKHR(vk::PresentInfoKHR(*render_finished_semaphore_list[current_frame_index], *swapchain, image_index));

		current_frame_index = (current_frame_index + 1) % framebuffer_list.size();
	}
};


} // namespace Vulkan

} // namespace ViewDesign

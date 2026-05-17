#pragma once

#include "ViewDesign/geometry/sizeu.h"
#include "ViewDesign/platform/vulkan/render_pass.h"
#include "ViewDesign/platform/vulkan/frame_in_flight.h"


namespace ViewDesign {

namespace Vulkan {


class Surface {
private:
	vk::raii::SurfaceKHR   surface;
	vk::Extent2D           extent;
	vk::raii::SwapchainKHR swapchain = nullptr;
	vk::raii::RenderPass   render_pass = nullptr;

private:
	static constexpr uint32_t image_count_default = 3;
private:
	std::vector<vk::raii::ImageView>   image_view_list;
	std::vector<vk::raii::Framebuffer> framebuffer_list;
	uint32_t                           image_index = 0;

private:
	static constexpr uint32_t frame_in_flight_count = 2;
private:
	std::vector<FrameInFlight> frame_in_flight_list;
	uint32_t                   frame_in_flight_index = 0;

public:
	Surface(vk::raii::SurfaceKHR surface, SizeU size) : surface(std::move(surface)) { Initialize(size); }

private:
	static vk::SurfaceFormatKHR SelectSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surface_format_list) {
		for (const auto& surface_format : surface_format_list) {
			if (surface_format.format == vk::Format::eB8G8R8A8Unorm && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return surface_format;
			}
		}
		return surface_format_list[0];
	}
	static vk::PresentModeKHR SelectPresentMode(const std::vector<vk::PresentModeKHR>& present_mode_list) {
		for (const auto& present_mode : present_mode_list) {
			if (present_mode == vk::PresentModeKHR::eMailbox) {
				return present_mode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}
	static vk::Extent2D ClampSizeToMinMaxExtent(SizeU size, const vk::SurfaceCapabilitiesKHR& surface_capabilities) {
		return vk::Extent2D(
			std::clamp(size.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width),
			std::clamp(size.height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height)
		);
	}
private:
	void Initialize(SizeU size) {
		DeviceContext& device_context = DeviceContext::Get(surface);
		vk::raii::PhysicalDevice& physical_device = device_context.physical_device;
		vk::raii::Device& device = device_context.device;

		vk::SurfaceCapabilitiesKHR surface_capabilities = physical_device.getSurfaceCapabilitiesKHR(surface);

		extent = surface_capabilities.currentExtent == vk::Extent2D(UINT32_MAX, UINT32_MAX) ? ClampSizeToMinMaxExtent(size, surface_capabilities) : surface_capabilities.currentExtent;

		uint32_t image_count = std::clamp(image_count_default, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);
		vk::SurfaceFormatKHR surface_format = SelectSurfaceFormat(physical_device.getSurfaceFormatsKHR(surface));
		vk::PresentModeKHR present_mode = SelectPresentMode(physical_device.getSurfacePresentModesKHR(surface));

		swapchain = device.createSwapchainKHR(vk::SwapchainCreateInfoKHR(
			{}, surface, image_count, surface_format.format, surface_format.colorSpace, extent, 1,
			vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
			{}, surface_capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
			present_mode, vk::True
		));

		vk::AttachmentDescription attachment({}, surface_format.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		vk::AttachmentReference attachment_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, attachment_reference);
		vk::SubpassDependency subpass_dependency(vk::SubpassExternal, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite, vk::DependencyFlagBits::eByRegion);
		render_pass = device.createRenderPass(vk::RenderPassCreateInfo({}, attachment, subpass, subpass_dependency));

		for (const auto& image : swapchain.getImages()) {
			image_view_list.emplace_back(device.createImageView(vk::ImageViewCreateInfo({}, image, vk::ImageViewType::e2D, surface_format.format, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))));
			framebuffer_list.emplace_back(device.createFramebuffer(vk::FramebufferCreateInfo({}, render_pass, *image_view_list.back(), extent.width, extent.height, 1)));
		}

		std::vector<vk::raii::CommandBuffer> command_buffer_list = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(device_context.command_pool, vk::CommandBufferLevel::ePrimary, frame_in_flight_count));
		for (size_t i = 0; i < command_buffer_list.size(); ++i) {
			frame_in_flight_list.emplace_back(
				std::move(command_buffer_list[i]),
				device.createSemaphore({}),
				device.createSemaphore({}),
				device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled))
			);
		}
	}

public:
	void RenderBegin() {
		DeviceContext& device_context = DeviceContext::Get();
		FrameInFlight& frame_in_flight = frame_in_flight_list[frame_in_flight_index];
		vk::raii::CommandBuffer& command_buffer = frame_in_flight.command_buffer;

		auto wait_result = device_context.device.waitForFences(*frame_in_flight.fence, vk::True, UINT64_MAX);
		device_context.device.resetFences(*frame_in_flight.fence);

		auto [acquire_result, image_index] = swapchain.acquireNextImage(UINT64_MAX, *frame_in_flight.semaphore_image_available);
		if (acquire_result == vk::Result::eErrorOutOfDateKHR) {
			throw std::runtime_error("Vulkan: recreate surface");
		}
		this->image_index = image_index;

		command_buffer.reset();
		command_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		FrameInFlight::SetCurrent(&frame_in_flight);
	}

	void Render(Rect clip_region, auto func) {
		DeviceContext& device_context = DeviceContext::Get();
		FrameInFlight& frame_in_flight = frame_in_flight_list[frame_in_flight_index];
		vk::raii::CommandBuffer& command_buffer = frame_in_flight.command_buffer;

		Vulkan::Render(render_pass, framebuffer_list[image_index], extent, clip_region, std::forward<decltype(func)>(func));

		command_buffer.end();
		FrameInFlight::ResetCurrent();

		vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		device_context.queue.submit(vk::SubmitInfo(*frame_in_flight.semaphore_image_available, stage, *frame_in_flight.command_buffer, *frame_in_flight.semaphore_render_finished), frame_in_flight.fence);
		auto present_result = device_context.queue.presentKHR(vk::PresentInfoKHR(*frame_in_flight.semaphore_render_finished, *swapchain, image_index));

		VertexBufferPool::Get().Recycle(std::move(frame_in_flight.vertex_buffer_list));

		frame_in_flight_index = (frame_in_flight_index + 1) % frame_in_flight_list.size();
	}
};


} // namespace Vulkan

} // namespace ViewDesign

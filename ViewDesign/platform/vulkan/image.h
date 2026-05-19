#pragma once

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {

namespace Vulkan {


inline void TransitionImageLayout(vk::raii::CommandBuffer& command_buffer, vk::Image image, vk::ImageLayout& image_layout, vk::ImageLayout image_layout_new) {
	if (image_layout == image_layout_new) { return; }
	static auto GetAccessAndStage = [](vk::ImageLayout layout) -> std::pair<vk::AccessFlags, vk::PipelineStageFlags> {
		switch (layout) {
		case vk::ImageLayout::eUndefined:              return { {},                                        vk::PipelineStageFlagBits::eTopOfPipe };
		case vk::ImageLayout::eTransferSrcOptimal:     return { vk::AccessFlagBits::eTransferRead,         vk::PipelineStageFlagBits::eTransfer };
		case vk::ImageLayout::eTransferDstOptimal:     return { vk::AccessFlagBits::eTransferWrite,        vk::PipelineStageFlagBits::eTransfer };
		case vk::ImageLayout::eShaderReadOnlyOptimal:  return { vk::AccessFlagBits::eShaderRead,           vk::PipelineStageFlagBits::eFragmentShader };
		case vk::ImageLayout::eColorAttachmentOptimal: return { vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eColorAttachmentOutput };
		case vk::ImageLayout::ePresentSrcKHR:          return { {},                                        vk::PipelineStageFlagBits::eBottomOfPipe };
		default: throw std::runtime_error("Vulkan: unknown image layout");
		}
	};
	auto [src_access, src_stage] = GetAccessAndStage(image_layout); auto [dst_access, dst_stage] = GetAccessAndStage(image_layout_new);
	command_buffer.pipelineBarrier(src_stage, dst_stage, {}, {}, {}, vk::ImageMemoryBarrier(src_access, dst_access, image_layout, image_layout_new, vk::QueueFamilyIgnored, vk::QueueFamilyIgnored, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));
	image_layout = image_layout_new;
}


} // namespace Vulkan

} // namespace ViewDesign

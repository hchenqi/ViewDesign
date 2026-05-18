#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/geometry/size.h"
#include "ViewDesign/platform/vulkan/sampler.h"
#include "ViewDesign/platform/vulkan/descriptor_pool.h"
#include "ViewDesign/platform/vulkan/frame_in_flight.h"


namespace ViewDesign {

namespace Vulkan {


class Texture {
public:
	static constexpr vk::Format format = vk::Format::eR8G8B8A8Unorm;
	static constexpr vk::SampleCountFlagBits sample_count_flag_bits = vk::SampleCountFlagBits::e1;

public:
	vk::Extent2D           extent;
	vk::raii::Image        image = nullptr;
	vk::ImageLayout        image_layout = vk::ImageLayout::eUndefined;
	vk::raii::ImageView    image_view = nullptr;
	vk::raii::DeviceMemory device_memory = nullptr;

public:
	struct Sampler {
		static vk::raii::Sampler Create() {
			return DeviceContext::Get().device.createSampler(vk::SamplerCreateInfo({}, vk::Filter::eLinear, vk::Filter::eLinear));
		}
	};
	struct DescriptorSetLayout {
		static vk::raii::DescriptorSetLayout Create() {
			vk::DescriptorSetLayoutBinding binding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr);
			return DeviceContext::Get().device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, binding));
		}
	};
public:
	vk::raii::DescriptorSet descriptor_set = nullptr;

public:
	Texture(const PixelBuffer& pixel_buffer) {
		CreateImageView(pixel_buffer.Size(), vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
		CopyPixelBuffer(pixel_buffer.PixelDataLength(), pixel_buffer.PixelData());
	}

	Texture(SizeU size) {
		CreateImageView(size, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment);
		Clear();
	}

public:
	void TransitionImageLayout(vk::raii::CommandBuffer& command_buffer, vk::ImageLayout image_layout_new) {
		if (image_layout == image_layout_new) { return; }
		static auto GetAccessAndStage = [](vk::ImageLayout layout) -> std::pair<vk::AccessFlags, vk::PipelineStageFlags> {
			switch (layout) {
			case vk::ImageLayout::eUndefined:              return { {},                                        vk::PipelineStageFlagBits::eTopOfPipe };
			case vk::ImageLayout::eTransferSrcOptimal:     return { vk::AccessFlagBits::eTransferRead,         vk::PipelineStageFlagBits::eTransfer };
			case vk::ImageLayout::eTransferDstOptimal:     return { vk::AccessFlagBits::eTransferWrite,        vk::PipelineStageFlagBits::eTransfer };
			case vk::ImageLayout::eShaderReadOnlyOptimal:  return { vk::AccessFlagBits::eShaderRead,           vk::PipelineStageFlagBits::eFragmentShader };
			case vk::ImageLayout::eColorAttachmentOptimal: return { vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eColorAttachmentOutput };
			default: throw std::runtime_error("Vulkan: unhandled image layout transition");
			}
		};
		auto [src_access, src_stage] = GetAccessAndStage(image_layout); auto [dst_access, dst_stage] = GetAccessAndStage(image_layout_new);
		command_buffer.pipelineBarrier(src_stage, dst_stage, {}, {}, {}, vk::ImageMemoryBarrier(src_access, dst_access, image_layout, image_layout_new, vk::QueueFamilyIgnored, vk::QueueFamilyIgnored, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));
		image_layout = image_layout_new;
	}

private:
	void CreateImageView(SizeU size, vk::ImageUsageFlags image_usage_flags) {
		DeviceContext& device_context = DeviceContext::Get();

		extent = vk::Extent2D(size.width, size.height);
		image = device_context.device.createImage(vk::ImageCreateInfo({}, vk::ImageType::e2D, format, vk::Extent3D(extent, 1), 1, 1, sample_count_flag_bits, vk::ImageTiling::eOptimal, image_usage_flags, vk::SharingMode::eExclusive));
		image_layout = vk::ImageLayout::eUndefined;
		image_view = device_context.device.createImageView(vk::ImageViewCreateInfo({}, image, vk::ImageViewType::e2D, format, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));

		vk::MemoryRequirements memory_requirements = image.getMemoryRequirements();
		device_memory = device_context.device.allocateMemory(vk::MemoryAllocateInfo(memory_requirements.size, device_context.FindMemoryType(memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)));
		image.bindMemory(device_memory, 0);

		descriptor_set = DescriptorPoolSet::Get().AllocateOne<DescriptorSetLayout>();
		vk::DescriptorImageInfo image_info(GetSampler<Sampler>(), image_view, vk::ImageLayout::eShaderReadOnlyOptimal);
		DeviceContext::Get().device.updateDescriptorSets(vk::WriteDescriptorSet(descriptor_set, 0, 1, vk::DescriptorType::eCombinedImageSampler, image_info), {});
	}

	void CopyPixelBuffer(size_t buffer_size_bytes, const void* buffer) {
		DeviceContext& device_context = DeviceContext::Get();
		vk::raii::CommandBuffer& command_buffer = FrameInFlight::GetCurrent().command_buffer;

		vk::raii::Buffer staging_buffer = device_context.device.createBuffer(vk::BufferCreateInfo({}, buffer_size_bytes, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive));
		vk::MemoryRequirements staging_memory_requirements = staging_buffer.getMemoryRequirements();
		vk::raii::DeviceMemory staging_memory = device_context.device.allocateMemory(vk::MemoryAllocateInfo(staging_memory_requirements.size, device_context.FindMemoryType(staging_memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));
		staging_buffer.bindMemory(staging_memory, 0);
		void* mapped = staging_memory.mapMemory(0, buffer_size_bytes);
		std::memcpy(mapped, buffer, buffer_size_bytes);
		staging_memory.unmapMemory();

		TransitionImageLayout(command_buffer, vk::ImageLayout::eTransferDstOptimal);
		command_buffer.copyBufferToImage(staging_buffer, image, image_layout, vk::BufferImageCopy(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), vk::Offset3D(0, 0, 0), vk::Extent3D(extent, 1)));
	}

	void Clear() {
		vk::raii::CommandBuffer& command_buffer = FrameInFlight::GetCurrent().command_buffer;

		TransitionImageLayout(command_buffer, vk::ImageLayout::eTransferDstOptimal);
		command_buffer.clearColorImage(image, image_layout, vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
	}
};


} // namespace Vulkan

} // namespace ViewDesign

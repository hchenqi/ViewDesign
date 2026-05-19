#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/geometry/size.h"
#include "ViewDesign/platform/vulkan/image.h"
#include "ViewDesign/platform/vulkan/sampler.h"
#include "ViewDesign/platform/vulkan/descriptor_pool.h"
#include "ViewDesign/platform/vulkan/frame_in_flight.h"


namespace ViewDesign {

namespace Vulkan {


class Texture {
public:
	vk::Format             format = vk::Format::eUndefined;
	vk::Extent2D           extent = {};
	vk::raii::Image        image = nullptr;
	vk::ImageLayout        image_layout = vk::ImageLayout::eUndefined;
	vk::raii::DeviceMemory device_memory = nullptr;
	vk::raii::ImageView    image_view = nullptr;

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
		CreateImage(vk::Format::eB8G8R8A8Unorm, pixel_buffer.Size(), vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
		CopyPixelBuffer(pixel_buffer.PixelDataLength(), pixel_buffer.PixelData());
	}
	Texture(SizeU size) {
		CreateImage(FrameInFlight::GetSurfaceFormat(), size, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment);
		Clear();
	}

private:
	void CreateImage(vk::Format format, SizeU size, vk::ImageUsageFlags image_usage_flags) {
		DeviceContext& device_context = DeviceContext::Get();

		this->format = format;
		extent = vk::Extent2D(size.width, size.height);
		image = device_context.device.createImage(vk::ImageCreateInfo({}, vk::ImageType::e2D, format, vk::Extent3D(extent, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, image_usage_flags, vk::SharingMode::eExclusive));
		image_layout = vk::ImageLayout::eUndefined;

		vk::MemoryRequirements memory_requirements = image.getMemoryRequirements();
		device_memory = device_context.device.allocateMemory(vk::MemoryAllocateInfo(memory_requirements.size, device_context.FindMemoryType(memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)));
		image.bindMemory(device_memory, 0);

		image_view = device_context.device.createImageView(vk::ImageViewCreateInfo({}, image, vk::ImageViewType::e2D, format, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));

		descriptor_set = DescriptorPoolSet::Get().AllocateOne<DescriptorSetLayout>();
		vk::DescriptorImageInfo image_info(GetSampler<Sampler>(), image_view, vk::ImageLayout::eShaderReadOnlyOptimal);
		DeviceContext::Get().device.updateDescriptorSets(vk::WriteDescriptorSet(descriptor_set, 0, 0, vk::DescriptorType::eCombinedImageSampler, image_info), {});
	}

	void CopyPixelBuffer(size_t size, const void* data) {
		DeviceContext& device_context = DeviceContext::Get();
		FrameInFlight& frame_in_flight = FrameInFlight::GetCurrent();
		vk::raii::CommandBuffer& command_buffer = frame_in_flight.command_buffer;

		StagingBuffer& staging_buffer = frame_in_flight.AppendStagingBuffer(size, data);

		TransitionImageLayout(command_buffer, image, image_layout, vk::ImageLayout::eTransferDstOptimal);
		command_buffer.copyBufferToImage(staging_buffer.buffer, image, image_layout, vk::BufferImageCopy(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), vk::Offset3D(0, 0, 0), vk::Extent3D(extent, 1)));
		TransitionImageLayout(command_buffer, image, image_layout, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void Clear() {
		vk::raii::CommandBuffer& command_buffer = FrameInFlight::GetCurrent().command_buffer;

		TransitionImageLayout(command_buffer, image, image_layout, vk::ImageLayout::eTransferDstOptimal);
		command_buffer.clearColorImage(image, image_layout, vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		TransitionImageLayout(command_buffer, image, image_layout, vk::ImageLayout::eShaderReadOnlyOptimal);
	}
};


} // namespace Vulkan

} // namespace ViewDesign

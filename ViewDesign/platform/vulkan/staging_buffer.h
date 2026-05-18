#pragma once

#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


struct StagingBuffer {
public:
	vk::raii::Buffer buffer = nullptr;
	vk::raii::DeviceMemory memory = nullptr;

public:
	StagingBuffer(size_t size, const void* data) {
		DeviceContext& device_context = DeviceContext::Get();

		buffer = device_context.device.createBuffer(vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive));

		vk::MemoryRequirements staging_memory_requirements = buffer.getMemoryRequirements();
		memory = device_context.device.allocateMemory(vk::MemoryAllocateInfo(staging_memory_requirements.size, device_context.FindMemoryType(staging_memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));
		buffer.bindMemory(memory, 0);

		void* mapped = memory.mapMemory(0, size);
		std::memcpy(mapped, data, size);
		memory.unmapMemory();
	}
};


} // namespace Vulkan

} // namespace ViewDesign

#pragma once

#include "ViewDesign/platform/vulkan/device.h"

#include <vector>


namespace ViewDesign {

namespace Vulkan {


struct VertexBuffer {
public:
	static constexpr size_t size = sizeof(float) * 1024;

public:
	vk::raii::Buffer buffer = nullptr;
	vk::raii::DeviceMemory memory = nullptr;
	void* mapped = nullptr;

public:
	VertexBuffer() {
		DeviceContext& device_context = DeviceContext::Get();

		buffer = device_context.device.createBuffer(vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eVertexBuffer));

		vk::MemoryRequirements vertex_memory_requirements = buffer.getMemoryRequirements();
		memory = device_context.device.allocateMemory(vk::MemoryAllocateInfo(vertex_memory_requirements.size, device_context.FindMemoryType(vertex_memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));
		buffer.bindMemory(memory, 0);

		mapped = memory.mapMemory(0, vk::WholeSize);
	}

	VertexBuffer(VertexBuffer&& other) = default;

	VertexBuffer& operator=(VertexBuffer&& other) = default;

	~VertexBuffer() {
		if (memory != nullptr) {
			memory.unmapMemory();
		}
	}
};


struct VertexBufferPool {
private:
	std::vector<VertexBuffer> free;
public:
	VertexBuffer Allocate() {
		if (free.empty()) {
			return VertexBuffer();
		} else {
			VertexBuffer vertex_buffer = std::move(free.back());
			free.pop_back();
			return vertex_buffer;
		}
	}
	void Recycle(std::vector<VertexBuffer> list) {
		free.insert(free.end(), std::make_move_iterator(list.begin()), std::make_move_iterator(list.end()));
	}

public:
	static VertexBufferPool& Get() {
		static VertexBufferPool pool;
		return pool;
	}
};


} // namespace Vulkan

} // namespace ViewDesign

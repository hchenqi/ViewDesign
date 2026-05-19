#pragma once

#include "ViewDesign/platform/vulkan/vertex_buffer.h"
#include "ViewDesign/platform/vulkan/staging_buffer.h"


namespace ViewDesign {

namespace Vulkan {


struct FrameInFlight {
public:
	vk::raii::CommandBuffer   command_buffer;
	vk::raii::Semaphore       semaphore_image_available;
	vk::raii::Fence           fence;

public:
	std::vector<VertexBuffer> vertex_buffer_list;
	size_t vertex_buffer_current_offset = 0;
public:
	std::pair<vk::Buffer, size_t> PushVertices(const void* data, size_t size) {
		if (size >= VertexBuffer::size) {
			throw std::invalid_argument("data size exceeding vertex buffer size");
		}
		if (vertex_buffer_list.empty() || vertex_buffer_current_offset + size > VertexBuffer::size) {
			vertex_buffer_list.emplace_back(VertexBufferPool::Get().Allocate());
			vertex_buffer_current_offset = 0;
		}
		size_t offset = vertex_buffer_current_offset;
		memcpy(static_cast<std::byte*>(vertex_buffer_list.back().mapped) + offset, data, size);
		vertex_buffer_current_offset += size;
		return std::make_pair(*vertex_buffer_list.back().buffer, offset);
	}

public:
	std::vector<StagingBuffer> staging_buffer_list;
public:
	StagingBuffer& AppendStagingBuffer(size_t size, const void* data) {
		return staging_buffer_list.emplace_back(size, data);
	}

private:
	inline static ref_ptr<FrameInFlight> current = nullptr;
	inline static vk::Format format = vk::Format::eUndefined;
public:
	static void SetCurrent(ref_ptr<FrameInFlight> frame_in_flight, vk::Format surface_format) { if (current != nullptr) { throw std::invalid_argument("current frame in flight not set"); } current = frame_in_flight; format = surface_format; }
	static void ResetCurrent() { if (current == nullptr) { throw std::invalid_argument("current frame in flight not set"); } current = nullptr; }
	static FrameInFlight& GetCurrent() { if (current == nullptr) { throw std::invalid_argument("current frame in flight not set"); } return *current; }
	static vk::Format GetSurfaceFormat() { if (current == nullptr) { throw std::invalid_argument("current frame in flight not set"); } return format; }
};


} // namespace Vulkan

} // namespace ViewDesign

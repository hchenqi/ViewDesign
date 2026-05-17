#pragma once

#include "ViewDesign/platform/vulkan/vertex_buffer.h"


namespace ViewDesign {

namespace Vulkan {


struct FrameInFlight {
public:
	vk::raii::CommandBuffer   command_buffer;
	vk::raii::Semaphore       semaphore_image_available;
	vk::raii::Semaphore       semaphore_render_finished;
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

private:
	inline static ref_ptr<FrameInFlight> current = nullptr;
public:
	static void SetCurrent(ref_ptr<FrameInFlight> frame_in_flight) { if (current != nullptr) { throw std::invalid_argument("current frame in flight not set"); } current = frame_in_flight; }
	static void ResetCurrent() { if (current == nullptr) { throw std::invalid_argument("current frame in flight not set"); } current = nullptr; }
	static FrameInFlight& GetCurrent() { if (current == nullptr) { throw std::invalid_argument("current frame in flight not set"); } return *current; }
};


} // namespace Vulkan

} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/drawing/helper.h"
#include "ViewDesign/platform/vulkan/frame_in_flight.h"
#include "ViewDesign/platform/vulkan/geometry_helper.h"
#include "ViewDesign/platform/vulkan/pipeline.h"


namespace ViewDesign {

namespace Vulkan {


class RenderContext {
public:
	RenderContext(FrameInFlight& frame_in_flight, vk::Extent2D extent) : frame_in_flight(frame_in_flight), size(AsSizeU(extent)) {
		CommandBuffer().setViewport(0, vk::Viewport(0.0f, 0.0f, size.width, size.height, 0.0f, 1.0f));

		UpdateSize<PipelineLayoutCommon>();

		clip_stack.emplace_back(point_i_zero, AsSizeU(extent));
		SetClip(clip_stack.back());
	}

private:
	FrameInFlight& frame_in_flight;
public:
	vk::raii::CommandBuffer& CommandBuffer() { return frame_in_flight.command_buffer; }

	// context
public:
	struct PipelineLayoutCommon {
		struct Context {
			struct {
				float size[2];
				float transform[6];
			} vertex;
		};

		static vk::raii::PipelineLayout Create() {
			vk::PushConstantRange range(vk::ShaderStageFlagBits::eVertex, offsetof(Context, vertex), sizeof(Context::vertex));
			return DeviceContext::Get().device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, {}, range));
		}
	};
private:
	Size size;
	Transform transform;
private:
	template<class PipelineLayout>
	void UpdateSize() {
		CommandBuffer().pushConstants<float>(*GetPipelineLayout<PipelineLayout>(), vk::ShaderStageFlagBits::eVertex, offsetof(typename PipelineLayout::Context, vertex.size), std::array<float, 2>{ size.width, size.height });
	}
	template<class PipelineLayout>
	void UpdateTransform() {
		CommandBuffer().pushConstants<float>(*GetPipelineLayout<PipelineLayout>(), vk::ShaderStageFlagBits::eVertex, offsetof(typename PipelineLayout::Context, vertex.transform), reinterpret_cast<const float(&)[6]>(this->transform.matrix));
	}
public:
	void SetTransform(const Transform& transform) {
		this->transform = transform;
		UpdateTransform<PipelineLayoutCommon>();
	}

	// clip
private:
	std::vector<RectI> clip_stack;
private:
	void SetClip(RectI rect) {
		CommandBuffer().setScissor(0, AsVulkanRect2D(rect));
	}
public:
	void PushClip(Rect clip_region) {
		RectI rect = clip_stack.back().Intersect(Round(GetBoundingRectAfterTransform(clip_region, transform)));
		if (rect != clip_stack.back()) {
			SetClip(rect);
		}
		clip_stack.emplace_back(rect);
	}
	void PopClip() {
		RectI rect = clip_stack.back();
		clip_stack.pop_back();
		if (clip_stack.back() != rect) {
			SetClip(clip_stack.back());
		}
	}

	// pipeline
private:
	ref_ptr<vk::raii::Pipeline> current_pipeline = nullptr;
public:
	template<class Pipeline> requires (std::derived_from<typename Pipeline::Layout::Context, PipelineLayoutCommon::Context>)
	void BindPipeline() {
		vk::raii::Pipeline& pipeline = GetPipeline<Pipeline>();
		if (current_pipeline != &pipeline) {
			CommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
			UpdateSize<typename Pipeline::Layout>();
			UpdateTransform<typename Pipeline::Layout>();
			current_pipeline = &pipeline;
		}
	}
public:
	void Clear() {}

public:
	std::pair<vk::Buffer, size_t> PushVertices(const void* data, size_t size) {
		return frame_in_flight.PushVertices(data, size);
	}
	void DrawVertices(auto vertices) {
		auto [vertex_buffer, offset] = PushVertices(vertices.data(), sizeof(vertices));
		CommandBuffer().bindVertexBuffers(0, vertex_buffer, { offset });
		CommandBuffer().draw(vertices.size(), 1, 0, 0);
	}
};


} // namespace Vulkan


struct RenderTarget : Vulkan::RenderContext {};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/drawing/helper.h"
#include "ViewDesign/platform/vulkan/frame_in_flight.h"
#include "ViewDesign/platform/vulkan/geometry_helper.h"
#include "ViewDesign/platform/vulkan/pipeline.h"
#include "ViewDesign/platform/vulkan/shader_module.h"
#include "ViewDesign/platform/vulkan/shaders/flat.vert.spv.h"
#include "ViewDesign/platform/vulkan/shaders/flat.frag.spv.h"


namespace ViewDesign {

namespace Vulkan {


struct FlatPipeline {
	struct Context {
		struct {
			float size[2];
			float transform[6];
		} vertex;

		struct {
			float color[4];
		} fragment;
	};

	struct VertexInput {
		float vertex[2];
	};

	static DeviceContext::PipelineInfo Create(vk::RenderPass render_pass) {
		vk::raii::Device& device = DeviceContext::Get().device;

		std::array<vk::PipelineShaderStageCreateInfo, 2> stage_list = {
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, GetShaderModule<Shaders::flat_vert>(), "main"),
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, GetShaderModule<Shaders::flat_frag>(), "main"),
		};

		vk::VertexInputBindingDescription binding(0, sizeof(VertexInput), vk::VertexInputRate::eVertex);
		vk::VertexInputAttributeDescription attribute(0, 0, vk::Format::eR32G32Sfloat, 0);
		vk::PipelineVertexInputStateCreateInfo vertex_input({}, 1, &binding, 1, &attribute);

		vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);

		vk::PipelineViewportStateCreateInfo viewport_state({}, 1, {}, 1, {});

		vk::PipelineRasterizationStateCreateInfo rasterizer({}, {}, {}, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, {}, {}, {}, {}, {}, 1.0f);

		vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1);

		vk::PipelineColorBlendAttachmentState blend_attachment(VK_TRUE, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		vk::PipelineColorBlendStateCreateInfo color_blending({}, {}, {}, 1, &blend_attachment);

		std::array<vk::DynamicState, 2> dynamic_state_list = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_state_list);

		std::array<vk::PushConstantRange, 2> range_list = {
			vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, offsetof(Context, vertex), sizeof(Context::vertex)),
			vk::PushConstantRange(vk::ShaderStageFlagBits::eFragment, offsetof(Context, fragment), sizeof(Context::fragment)),
		};
		vk::PipelineLayoutCreateInfo layout_info({}, {}, range_list);
		vk::raii::PipelineLayout layout = device.createPipelineLayout(layout_info);

		vk::raii::Pipeline pipeline = device.createGraphicsPipeline(nullptr, vk::GraphicsPipelineCreateInfo(
			{},
			stage_list,
			&vertex_input,
			&input_assembly,
			{},
			&viewport_state,
			&rasterizer,
			&multisampling,
			{},
			&color_blending,
			&dynamic_state,
			layout,
			render_pass
		));

		return std::make_pair(std::move(layout), std::move(pipeline));
	}
};


struct RenderContext {
private:
	FrameInFlight& frame_in_flight;
	DeviceContext::PipelineInfo& flat_pipeline;
	vk::Extent2D extent;
	Transform current_transform;
	std::vector<RectI> clip_stack;

public:
	RenderContext(FrameInFlight& frame_in_flight, vk::RenderPass render_pass, vk::Extent2D extent) : frame_in_flight(frame_in_flight), flat_pipeline(GetPipelineInfo<FlatPipeline>(render_pass)), extent(extent) {
		CommandBuffer().setViewport(0, vk::Viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f));

		float context_size[] = { (float)extent.width, (float)extent.height };
		CommandBuffer().pushConstants<float>(flat_pipeline.first, vk::ShaderStageFlagBits::eVertex, offsetof(FlatPipeline::Context, vertex.size), context_size);
	}

public:
	vk::raii::CommandBuffer& CommandBuffer() { return frame_in_flight.command_buffer; }

public:
	void SetTransform(const Transform& transform) {
		current_transform = transform;
		CommandBuffer().pushConstants<float>(flat_pipeline.first, vk::ShaderStageFlagBits::eVertex, offsetof(FlatPipeline::Context, vertex.transform), reinterpret_cast<const float(&)[6]>(current_transform.matrix));
	}

	void SetColor(Color color) {
		auto [r, g, b, a] = AsTupleNormalizedPremultiplied(color);
		float context_color[] = { r, g, b, a };
		CommandBuffer().pushConstants<float>(flat_pipeline.first, vk::ShaderStageFlagBits::eFragment, offsetof(FlatPipeline::Context, fragment.color), context_color);
	}

private:
	void SetClip(RectI rect) {
		CommandBuffer().setScissor(0, AsVulkanRect2D(rect));
	}
public:
	void PushClip(Rect clip_region) {
		RectI rect = Round(GetBoundingRectAfterTransform(clip_region, current_transform));
		SetClip(rect);
		clip_stack.push_back(rect);
	}
	void PopClip() {
		clip_stack.pop_back();
		if (!clip_stack.empty()) {
			SetClip(clip_stack.back());
		}
	}

public:
	void Clear() {}

private:
	ref_ptr<DeviceContext::PipelineInfo> current_pipeline = nullptr;
public:
	void BindFlatPipeline() {
		if (current_pipeline != &flat_pipeline) {
			CommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, flat_pipeline.second);
			current_pipeline = &flat_pipeline;
		}
	}

public:
	std::pair<vk::Buffer, size_t> PushVertices(const void* data, size_t size) { return frame_in_flight.PushVertices(data, size); }
	std::pair<vk::Buffer, size_t> PushVertices(const auto& data) { return PushVertices(data, sizeof(data)); }
};


} // namespace Vulkan


struct RenderTarget : Vulkan::RenderContext {};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/drawing/helper.h"
#include "ViewDesign/platform/vulkan/frame_in_flight.h"
#include "ViewDesign/platform/vulkan/geometry_helper.h"
#include "ViewDesign/platform/vulkan/texture.h"
#include "ViewDesign/platform/vulkan/pipeline.h"
#include "ViewDesign/platform/vulkan/shader_module.h"
#include "ViewDesign/platform/vulkan/shaders/flat.vert.spv.h"
#include "ViewDesign/platform/vulkan/shaders/flat.frag.spv.h"
#include "ViewDesign/platform/vulkan/shaders/composite.vert.spv.h"
#include "ViewDesign/platform/vulkan/shaders/composite.frag.spv.h"


namespace ViewDesign {

namespace Vulkan {


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


struct FlatPipeline {
public:
	using VertexShader = Shaders::flat_vert;
	using FragmentShader = Shaders::flat_frag;

	struct Layout {
		struct Context : PipelineLayoutCommon::Context {
			struct {
				float color[4];
			} fragment;
		};

		static vk::raii::PipelineLayout Create() {
			std::array range_list = {
				vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, offsetof(Context, vertex), sizeof(Context::vertex)),
				vk::PushConstantRange(vk::ShaderStageFlagBits::eFragment, offsetof(Context, fragment), sizeof(Context::fragment)),
			};
			return DeviceContext::Get().device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, {}, range_list));
		}
	};

	struct Input {
		float point[2];
	};

public:
	static vk::raii::Pipeline Create(vk::RenderPass render_pass) {
		std::array stage_list = {
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, GetShaderModule<VertexShader>(), "main"),
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, GetShaderModule<FragmentShader>(), "main"),
		};

		vk::VertexInputBindingDescription binding(0, sizeof(Input), vk::VertexInputRate::eVertex);
		vk::VertexInputAttributeDescription attribute(0, 0, vk::Format::eR32G32Sfloat, 0);
		vk::PipelineVertexInputStateCreateInfo vertex_input({}, binding, attribute);

		vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);
		vk::PipelineViewportStateCreateInfo viewport_state({}, 1, {}, 1, {});
		vk::PipelineRasterizationStateCreateInfo rasterizer({}, {}, {}, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, {}, {}, {}, {}, {}, 1.0f);
		vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1);

		vk::PipelineColorBlendAttachmentState blend_attachment(VK_TRUE, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		vk::PipelineColorBlendStateCreateInfo color_blending({}, {}, {}, blend_attachment);

		std::array dynamic_state_list = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_state_list);

		return DeviceContext::Get().device.createGraphicsPipeline(nullptr, vk::GraphicsPipelineCreateInfo({}, stage_list, &vertex_input, &input_assembly, {}, &viewport_state, &rasterizer, &multisampling, {}, &color_blending, &dynamic_state, GetPipelineLayout<Layout>(), render_pass));
	}
};


struct CompositePipeline {
public:
	using VertexShader = Shaders::composite_vert;
	using FragmentShader = Shaders::composite_frag;

	struct Layout {
		struct Context : PipelineLayoutCommon::Context {
			struct {
				float opacity;
			} fragment;
		};

		static vk::raii::PipelineLayout Create() {
			std::array range_list = {
				vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, offsetof(Context, vertex), sizeof(Context::vertex)),
				vk::PushConstantRange(vk::ShaderStageFlagBits::eFragment, offsetof(Context, fragment), sizeof(Context::fragment)),
			};
			return DeviceContext::Get().device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, *GetDescriptorSetLayout<Texture::DescriptorSetLayout>(), range_list));
		}
	};

	struct Input {
		float point[2];
		float uv[2];
	};

public:
	static vk::raii::Pipeline Create(vk::RenderPass render_pass) {
		std::array stage_list = {
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, GetShaderModule<VertexShader>(), "main"),
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, GetShaderModule<FragmentShader>(), "main"),
		};

		vk::VertexInputBindingDescription binding(0, sizeof(Input), vk::VertexInputRate::eVertex);
		std::array attribute_list = {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Input, point)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(Input, uv)),
		};
		vk::PipelineVertexInputStateCreateInfo vertex_input({}, binding, attribute_list);

		vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);
		vk::PipelineViewportStateCreateInfo viewport_state({}, 1, {}, 1, {});
		vk::PipelineRasterizationStateCreateInfo rasterizer({}, {}, {}, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, {}, {}, {}, {}, {}, 1.0f);
		vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1);

		vk::PipelineColorBlendAttachmentState blend_attachment(VK_TRUE, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		vk::PipelineColorBlendStateCreateInfo color_blending({}, {}, {}, blend_attachment);

		std::array dynamic_state_list = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_state_list);

		return DeviceContext::Get().device.createGraphicsPipeline(nullptr, vk::GraphicsPipelineCreateInfo({}, stage_list, &vertex_input, &input_assembly, {}, &viewport_state, &rasterizer, &multisampling, {}, &color_blending, &dynamic_state, GetPipelineLayout<Layout>(), render_pass));
	}
};


class RenderContext {
public:
	RenderContext(FrameInFlight& frame_in_flight, vk::RenderPass render_pass, vk::Extent2D extent) : frame_in_flight(frame_in_flight), render_pass(render_pass), size(extent.width, extent.height) {
		CommandBuffer().setViewport(0, vk::Viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f));

		float context_size[] = { (float)extent.width, (float)extent.height };
		CommandBuffer().pushConstants<float>(GetPipelineLayout<PipelineLayoutCommon>(), vk::ShaderStageFlagBits::eVertex, offsetof(PipelineLayoutCommon::Context, vertex.size), context_size);
	}

private:
	FrameInFlight& frame_in_flight;
	vk::RenderPass render_pass;
public:
	vk::raii::CommandBuffer& CommandBuffer() { return frame_in_flight.command_buffer; }

	// context
private:
	Size size;
	Transform transform;
public:
	void SetTransform(const Transform& transform) {
		this->transform = transform;
		CommandBuffer().pushConstants<float>(GetPipelineLayout<PipelineLayoutCommon>(), vk::ShaderStageFlagBits::eVertex, offsetof(PipelineLayoutCommon::Context, vertex.transform), reinterpret_cast<const float(&)[6]>(this->transform.matrix));
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
		RectI rect = Round(GetBoundingRectAfterTransform(clip_region, transform));
		SetClip(rect);
		clip_stack.push_back(rect);
	}
	void PopClip() {
		clip_stack.pop_back();
		if (!clip_stack.empty()) {
			SetClip(clip_stack.back());
		}
	}

	// pipeline
private:
	ref_ptr<vk::raii::Pipeline> current_pipeline = nullptr;
public:
	template<class Pipeline>
	void BindPipeline() {
		vk::raii::Pipeline& pipeline = GetPipeline<Pipeline>(render_pass);
		if (current_pipeline != &pipeline) {
			CommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
			current_pipeline = &pipeline;
		}
	}
public:
	void Clear() {}
	void SetColor(Color color) {
		auto [r, g, b, a] = AsTupleNormalizedPremultiplied(color);
		float context_color[] = { r, g, b, a };
		CommandBuffer().pushConstants<float>(GetPipelineLayout<FlatPipeline::Layout>(), vk::ShaderStageFlagBits::eFragment, offsetof(FlatPipeline::Layout::Context, fragment.color), context_color);
	}
	void SetOpacity(float opacity) {
		CommandBuffer().pushConstants<float>(GetPipelineLayout<CompositePipeline::Layout>(), vk::ShaderStageFlagBits::eFragment, offsetof(CompositePipeline::Layout::Context, fragment.opacity), opacity);
	}

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

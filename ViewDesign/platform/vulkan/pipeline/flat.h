#pragma once

#include "ViewDesign/platform/vulkan/render_target.h"
#include "ViewDesign/platform/vulkan/shader_module.h"
#include "ViewDesign/platform/vulkan/shader/flat.vert.spv.h"
#include "ViewDesign/platform/vulkan/shader/flat.frag.spv.h"


namespace ViewDesign {

namespace Vulkan {


struct FlatPipeline {
public:
	using VertexShader = Shaders::flat_vert;
	using FragmentShader = Shaders::flat_frag;

	struct Layout {
		struct Context : RenderContext::PipelineLayoutCommon::Context {
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
	static vk::raii::Pipeline Create() {
		std::array stage_list = {
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, *GetShaderModule<VertexShader>(), "main"),
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, *GetShaderModule<FragmentShader>(), "main"),
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

		vk::Format color_attachment_format = FrameInFlight::GetSurfaceFormat();
		vk::PipelineRenderingCreateInfo rendering_info(0, color_attachment_format);

		return DeviceContext::Get().device.createGraphicsPipeline(nullptr, vk::GraphicsPipelineCreateInfo({}, stage_list, &vertex_input, &input_assembly, {}, &viewport_state, &rasterizer, &multisampling, {}, &color_blending, &dynamic_state, *GetPipelineLayout<Layout>(), {}, {}, {}, {}, &rendering_info));
	}

public:
	static void SetColor(RenderContext& context, Color color) {
		context.CommandBuffer().pushConstants<float>(*GetPipelineLayout<Layout>(), vk::ShaderStageFlagBits::eFragment, offsetof(Layout::Context, fragment.color), AsArray(AsTupleNormalizedPremultiplied(color)));
	}
};


} // namespace Vulkan

} // namespace ViewDesign

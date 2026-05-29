#pragma once

#include <ViewDesign/platform/vulkan/pipeline.h>
#include <ViewDesign/platform/vulkan/shader_module.h>
#include <ViewDesign/platform/vulkan/frame_in_flight.h>

#include <glm/glm.hpp>

#include "../shader/tetra.vert.spv.h"
#include "../shader/tetra.frag.spv.h"


using namespace ViewDesign::Vulkan;


struct TetraPipeline {
public:
	using VertexShader = tetra_vert;
	using FragmentShader = tetra_frag;

	struct Layout {
		struct Context {
			struct {
				glm::mat4 mvp;
			} vertex;
		};

		static vk::raii::PipelineLayout Create() {
			vk::PushConstantRange range(vk::ShaderStageFlagBits::eVertex, offsetof(Context, vertex), sizeof(Context::vertex));
			return DeviceContext::Get().device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, {}, range));
		}
	};

	struct Input {
		float pos[3];
		float col[3];
	};

public:
	static vk::raii::Pipeline Create() {
		std::array stage_list = {
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, *GetShaderModule<VertexShader>(), "main"),
			vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, *GetShaderModule<FragmentShader>(), "main"),
		};

		vk::VertexInputBindingDescription binding(0, sizeof(Input), vk::VertexInputRate::eVertex);
		std::array attribute_list = {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Input, pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Input, col)),
		};
		vk::PipelineVertexInputStateCreateInfo vertex_input({}, binding, attribute_list);

		vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);
		vk::PipelineViewportStateCreateInfo viewport_state({}, 1, {}, 1, {});
		vk::PipelineRasterizationStateCreateInfo rasterizer({}, {}, {}, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, {}, {}, {}, {}, {}, 1.0f);
		vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1);

		vk::PipelineDepthStencilStateCreateInfo depth_stencil({}, true, true, vk::CompareOp::eLess, false, false, {}, {}, 0.0f, 1.0f);

		vk::PipelineColorBlendAttachmentState blend_attachment(VK_TRUE, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		vk::PipelineColorBlendStateCreateInfo color_blending({}, {}, {}, blend_attachment);

		std::array dynamic_state_list = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_state_list);

		vk::Format color_attachment_format = FrameInFlight::GetSurfaceFormat();
		vk::PipelineRenderingCreateInfo rendering_info(0, color_attachment_format, DeviceContext::Get().FindDepthFormat());

		return DeviceContext::Get().device.createGraphicsPipeline(nullptr, vk::GraphicsPipelineCreateInfo({}, stage_list, &vertex_input, &input_assembly, {}, &viewport_state, &rasterizer, &multisampling, &depth_stencil, &color_blending, &dynamic_state, *GetPipelineLayout<Layout>(), {}, {}, {}, {}, &rendering_info));
	}
};

#pragma once

#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


template<class Pipeline>
inline std::pair<vk::raii::PipelineLayout, vk::raii::Pipeline>& GetPipelineInfo(vk::RenderPass render_pass) {
	static size_t pipeline_index = DeviceContext::Get().CreatePipeline<Pipeline>(render_pass);
	return DeviceContext::Get().GetPipelineInfo(pipeline_index);
}


} // namespace Vulkan

} // namespace ViewDesign

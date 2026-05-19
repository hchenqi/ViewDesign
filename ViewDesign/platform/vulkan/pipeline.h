#pragma once

#include "ViewDesign/platform/vulkan/registry.h"

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {

namespace Vulkan {


template<class PipelineLayout>
inline vk::raii::PipelineLayout& GetPipelineLayout() {
	static size_t index = Registry<vk::raii::PipelineLayout>::Get().PutItem(PipelineLayout::Create());
	return Registry<vk::raii::PipelineLayout>::Get().GetItem(index);
}


template<class Pipeline>
inline vk::raii::Pipeline& GetPipeline() {
	static size_t index = Registry<vk::raii::Pipeline>::Get().PutItem(Pipeline::Create());
	return Registry<vk::raii::Pipeline>::Get().GetItem(index);
}


} // namespace Vulkan

} // namespace ViewDesign

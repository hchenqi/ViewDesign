#pragma once

#include "ViewDesign/platform/vulkan/registry.h"
#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


template<class Sampler>
vk::raii::Sampler& GetSampler() {
	static size_t index = Registry<vk::raii::Sampler>::Get().PutItem(Sampler::Create());
	return Registry<vk::raii::Sampler>::Get().GetItem(index);
}


} // namespace Vulkan

} // namespace ViewDesign

#pragma once

#include "ViewDesign/platform/vulkan/registry.h"
#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


template<class Shader>
inline vk::raii::ShaderModule& GetShaderModule() {
	static size_t index = Registry<vk::raii::ShaderModule>::Get().PutItem(DeviceContext::Get().device.createShaderModule(vk::ShaderModuleCreateInfo({}, Shader::code)));
	return Registry<vk::raii::ShaderModule>::Get().GetItem(index);
}


} // namespace Vulkan

} // namespace ViewDesign

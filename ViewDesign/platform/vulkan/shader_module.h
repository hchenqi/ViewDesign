#pragma once

#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


template<class Shader>
inline vk::raii::ShaderModule& GetShaderModule() {
	static size_t shader_module_index = DeviceContext::Get().LoadShaderModule(Shader::code);
	return DeviceContext::Get().GetShaderModule(shader_module_index);
}


} // namespace Vulkan

} // namespace ViewDesign

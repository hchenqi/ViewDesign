#pragma once

#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


class ShaderModuleRegistry {
private:
	std::vector<vk::raii::ShaderModule> shader_module_list;

public:
	size_t LoadShaderModule(const auto& code) {
		shader_module_list.emplace_back(DeviceContext::Get().device.createShaderModule(vk::ShaderModuleCreateInfo({}, code)));
		return shader_module_list.size() - 1;
	}
	vk::raii::ShaderModule& GetShaderModule(size_t shader_module_index) {
		return shader_module_list[shader_module_index];
	}

public:
	static ShaderModuleRegistry& Get() {
		static ShaderModuleRegistry registry;
		return registry;
	}
};


template<class Shader>
inline vk::raii::ShaderModule& GetShaderModule() {
	static size_t shader_module_index = ShaderModuleRegistry::Get().LoadShaderModule(Shader::code);
	return ShaderModuleRegistry::Get().GetShaderModule(shader_module_index);
}


} // namespace Vulkan

} // namespace ViewDesign

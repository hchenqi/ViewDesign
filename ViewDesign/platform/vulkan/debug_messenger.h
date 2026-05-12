#pragma once

#include "ViewDesign/platform/vulkan/instance.h"


namespace ViewDesign {

namespace Vulkan {


class DebugMessengerContext {
private:
	vk::raii::DebugUtilsMessengerEXT debug_messenger = nullptr;
private:
	DebugMessengerContext() {
		using Severity = vk::DebugUtilsMessageSeverityFlagBitsEXT;
		using Type = vk::DebugUtilsMessageTypeFlagBitsEXT;
		debug_messenger = InstanceContext::GetInstance().createDebugUtilsMessengerEXT(vk::DebugUtilsMessengerCreateInfoEXT(
			{},
			Severity::eVerbose | Severity::eWarning | Severity::eError,
			Type::eGeneral | Type::eValidation | Type::ePerformance,
			Callback
		));
	}
private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* data, void*) {
		if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			fprintf(stderr, "[Vulkan] %s\n", data->pMessage);
		}
		return vk::False;
	}
public:
	inline static void Initialize() {
		static DebugMessengerContext context;
	}
};


} // namespace Vulkan

} // namespace ViewDesign

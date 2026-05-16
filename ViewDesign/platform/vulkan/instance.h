#pragma once

#include "ViewDesign/common/type.h"

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {

namespace Vulkan {


class InstanceContext {
private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
private:
	InstanceContext(const std::vector<const char*>& layers, const std::vector<const char*>& extensions) {
		vk::ApplicationInfo app_info("", 1, "", 1, vk::ApiVersion13);
		instance = context.createInstance(vk::InstanceCreateInfo({}, &app_info, layers, extensions));
	}

private:
	inline static ref_ptr<InstanceContext> ref = nullptr;
public:
	static vk::raii::Instance& Initialize(const std::vector<const char*>& layers, const std::vector<const char*>& extensions) {
		static InstanceContext context(layers, extensions);
		if (ref == nullptr) {
			ref = &context;
		} else {
			throw std::invalid_argument("Vulkan: instance context already initialized");
		}
		return ref->instance;
	}
	static vk::raii::Instance& GetInstance() {
		if (ref == nullptr) {
			throw std::invalid_argument("Vulkan: instance context not initialized");
		}
		return ref->instance;
	}
};


} // namespace Vulkan

} // namespace ViewDesign

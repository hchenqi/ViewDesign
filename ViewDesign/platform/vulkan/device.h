#pragma once

#include "ViewDesign/platform/vulkan/instance.h"


namespace ViewDesign {

namespace Vulkan {


class DeviceContext {
public:
	vk::raii::PhysicalDevice physical_device = nullptr;
	uint32_t queue_family_index = -1;
	vk::raii::Device device = nullptr;
	vk::raii::Queue queue = nullptr;
	vk::raii::CommandPool command_pool = nullptr;

private:
	DeviceContext(const vk::raii::SurfaceKHR& surface) {
		std::tie(physical_device, queue_family_index) = FindSuitablePhysicalDevice(vk::raii::PhysicalDevices(InstanceContext::GetInstance()), surface);

		float queue_priority = 1.0f;
		vk::DeviceQueueCreateInfo queue_create_info({}, queue_family_index, 1, &queue_priority);
		std::vector<const char*> extensions = { vk::KHRSwapchainExtensionName, vk::KHRDynamicRenderingExtensionName };
		vk::PhysicalDeviceDynamicRenderingFeatures dynamic_rendering(vk::True);
		device = physical_device.createDevice(vk::DeviceCreateInfo({}, queue_create_info, {}, extensions, {}, &dynamic_rendering));

		queue = device.getQueue(queue_family_index, 0);
		command_pool = device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queue_family_index));
	}

private:
	static std::pair<vk::raii::PhysicalDevice, uint32_t> FindSuitablePhysicalDevice(const vk::raii::PhysicalDevices& physical_device_list, const vk::raii::SurfaceKHR& surface) {
		for (const auto& physical_device : physical_device_list) {
			std::vector<vk::QueueFamilyProperties> queue_family_properties_list = physical_device.getQueueFamilyProperties();
			for (size_t i = 0; i < queue_family_properties_list.size(); ++i) {
				if ((queue_family_properties_list[i].queueFlags & vk::QueueFlagBits::eGraphics) && physical_device.getSurfaceSupportKHR(i, *surface)) {
					return std::make_pair(physical_device, (uint32_t)i);
				}
			}
		}
		throw std::runtime_error("Vulkan: failed to find a suitable device");
	}

private:
	inline static ref_ptr<DeviceContext> ref = nullptr;
public:
	static DeviceContext& Get(const vk::raii::SurfaceKHR& surface) {
		static DeviceContext context(surface);
		if (ref == nullptr) {
			ref = &context;
		} else {
			if (!ref->physical_device.getSurfaceSupportKHR(ref->queue_family_index, *surface)) {
				throw std::runtime_error("Vulkan: surface not supported by current device");
			}
		}
		return *ref;
	}
	static DeviceContext& Get() {
		if (ref == nullptr) {
			throw std::logic_error("Vulkan: device not created");
		}
		return *ref;
	}

public:
	uint32_t FindMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags required_properties) {
		vk::PhysicalDeviceMemoryProperties memory_properties = physical_device.getMemoryProperties();
		for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
			if ((type_filter & (1u << i)) && (memory_properties.memoryTypes[i].propertyFlags & required_properties) == required_properties) { return i; }
		}
		throw std::runtime_error("Vulkan: no suitable memory type");
	}
};


} // namespace Vulkan

} // namespace ViewDesign

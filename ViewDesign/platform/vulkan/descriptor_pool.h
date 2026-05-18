#pragma once

#include "ViewDesign/platform/vulkan/device.h"

#include <vector>


namespace ViewDesign {

namespace Vulkan {


template<class DescriptorSetLayout>
inline vk::raii::DescriptorSetLayout& GetDescriptorSetLayout() {
	static size_t index = Registry<vk::raii::DescriptorSetLayout>::Get().PutItem(DescriptorSetLayout::Create());
	return Registry<vk::raii::DescriptorSetLayout>::Get().GetItem(index);
}


struct DescriptorPool {
public:
	static constexpr size_t size = 64;
public:
	vk::raii::DescriptorPool pool = nullptr;
public:
	DescriptorPool() {
		std::array pool_size_list = {
			vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, size),
		};
		pool = DeviceContext::Get().device.createDescriptorPool(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, size, pool_size_list));
	}
public:
	template<class DescriptorSetLayout>
	vk::raii::DescriptorSet AllocateOne() {
		return std::move(DeviceContext::Get().device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(pool, *GetDescriptorSetLayout<DescriptorSetLayout>())).back());
	}
};


struct DescriptorPoolSet {
private:
	std::vector<DescriptorPool> set;
public:
	template<class DescriptorSetLayout>
	vk::raii::DescriptorSet AllocateOne() {
		for (DescriptorPool& pool : set) {
			try {
				return pool.AllocateOne<DescriptorSetLayout>();
			} catch (vk::OutOfPoolMemoryError&) {}
		}
		return set.emplace_back().AllocateOne<DescriptorSetLayout>();
	}
public:
	static DescriptorPoolSet& Get() {
		static DescriptorPoolSet set;
		return set;
	}
};


} // namespace Vulkan

} // namespace ViewDesign

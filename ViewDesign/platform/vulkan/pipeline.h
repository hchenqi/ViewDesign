#pragma once

#include "ViewDesign/platform/vulkan/device.h"


namespace ViewDesign {

namespace Vulkan {


class DescriptorSetLayoutRegistry {
private:
	std::vector<vk::raii::DescriptorSetLayout> descriptor_set_layout_list;
public:
	template<class DescriptorSetLayout>
	size_t CreateDescriptorSetLayout() {
		descriptor_set_layout_list.emplace_back(DescriptorSetLayout::Create());
		return descriptor_set_layout_list.size() - 1;
	}
	vk::raii::DescriptorSetLayout& GetDescriptorSetLayout(size_t index) {
		return descriptor_set_layout_list[index];
	}
public:
	static DescriptorSetLayoutRegistry& Get() {
		static DescriptorSetLayoutRegistry registry;
		return registry;
	}
};

template<class DescriptorSetLayout>
inline vk::raii::DescriptorSetLayout& GetDescriptorSetLayout() {
	static size_t index = DescriptorSetLayoutRegistry::Get().CreateDescriptorSetLayout<DescriptorSetLayout>();
	return DescriptorSetLayoutRegistry::Get().GetDescriptorSetLayout(index);
}


class PipelineLayoutRegistry {
private:
	std::vector<vk::raii::PipelineLayout> pipeline_layout_list;
public:
	template<class PipelineLayout>
	size_t CreatePipelineLayout() {
		pipeline_layout_list.emplace_back(PipelineLayout::Create());
		return pipeline_layout_list.size() - 1;
	}
	vk::raii::PipelineLayout& GetPipelineLayout(size_t index) {
		return pipeline_layout_list[index];
	}
public:
	static PipelineLayoutRegistry& Get() {
		static PipelineLayoutRegistry registry;
		return registry;
	}
};

template<class PipelineLayout>
inline vk::raii::PipelineLayout& GetPipelineLayout() {
	static size_t index = PipelineLayoutRegistry::Get().CreatePipelineLayout<PipelineLayout>();
	return PipelineLayoutRegistry::Get().GetPipelineLayout(index);
}


class PipelineRegistry {
private:
	std::vector<vk::raii::Pipeline> pipeline_list;
public:
	template<class Pipeline>
	size_t CreatePipeline(vk::RenderPass render_pass) {
		pipeline_list.emplace_back(Pipeline::Create(render_pass));
		return pipeline_list.size() - 1;
	}
	vk::raii::Pipeline& GetPipeline(size_t index) {
		return pipeline_list[index];
	}
public:
	static PipelineRegistry& Get() {
		static PipelineRegistry registry;
		return registry;
	}
};

template<class Pipeline>
inline vk::raii::Pipeline& GetPipeline(vk::RenderPass render_pass) {
	static size_t index = PipelineRegistry::Get().CreatePipeline<Pipeline>(render_pass);
	return PipelineRegistry::Get().GetPipeline(index);
}


} // namespace Vulkan

} // namespace ViewDesign

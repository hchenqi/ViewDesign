#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/platform/vulkan/geometry_helper.h"

#include <vector>

#include <vulkan/vulkan_raii.hpp>


namespace ViewDesign {


struct RenderTarget : public vk::raii::CommandBuffer {
private:
	Transform current_transform;
	std::vector<RectI> clip_stack;

public:
	RenderTarget(CommandBuffer&& command_buffer) : CommandBuffer(std::move(command_buffer)) {}

public:
	void SetTransform(const Transform& transform) {
		current_transform = transform;
		const auto& mat32 = transform.matrix;
		float mat44[16] = {
			mat32[0][0], mat32[0][1], 0.0f, 0.0f,
			mat32[1][0], mat32[1][1], 0.0f, 0.0f,
			0.0f,        0.0f,        1.0f, 0.0f,
			mat32[2][0], mat32[2][1], 0.0f, 1.0f,
		};

		// load
	}

private:
	void SetClip(RectI rect) {
		// initialize pipeline

		setScissor(0, Vulkan::AsVulkanRect2D(rect));
	}
public:
	void PushClip(Rect clip_region) {
		RectI rect = Round(GetBoundingRectAfterTransform(clip_region, current_transform));
		SetClip(rect);
		clip_stack.push_back(rect);
	}
	void PopClip() {
		clip_stack.pop_back();
		if (!clip_stack.empty()) {
			SetClip(clip_stack.back());
		}
	}

public:
	void Clear() {}
};


} // namespace ViewDesign

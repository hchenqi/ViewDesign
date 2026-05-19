#pragma once

#include "ViewDesign/platform/vulkan/texture.h"
#include "ViewDesign/platform/vulkan/rendering.h"


namespace ViewDesign {

namespace Vulkan {


class Framebuffer : public Texture {
public:
	Framebuffer(SizeU size) : Texture(size) {}

public:
	void Render(RectI clip_region, auto func) {
		Vulkan::Render(image, image_layout, image_view, vk::ImageLayout::eShaderReadOnlyOptimal, extent, clip_region, std::forward<decltype(func)>(func));
	}
};


} // namespace Vulkan

} // namespace ViewDesign

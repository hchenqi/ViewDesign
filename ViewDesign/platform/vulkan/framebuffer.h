#pragma once

#include "ViewDesign/platform/vulkan/texture.h"
#include "ViewDesign/platform/vulkan/rendering.h"
#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

namespace Vulkan {


class Framebuffer : public Texture {
public:
	Framebuffer(SizeU size) : Texture(size) {}

public:
	void Render(RectI clip_region, auto func) {
		FrameInFlight& frame_in_flight = FrameInFlight::GetCurrent();
		vk::raii::CommandBuffer& command_buffer = frame_in_flight.command_buffer;

		Vulkan::Render(command_buffer, *image, image_layout, image_view, vk::ImageLayout::eShaderReadOnlyOptimal, extent, AsVulkanRect2D(clip_region), [&]() {
			RenderContext context(frame_in_flight, extent);
			func(static_cast<RenderTarget&>(context));
		});
	}
};


} // namespace Vulkan

} // namespace ViewDesign

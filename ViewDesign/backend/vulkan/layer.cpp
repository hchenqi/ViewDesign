#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/platform/vulkan/framebuffer.h"


namespace ViewDesign {

using namespace Vulkan;


void Layer::CreateFramebuffer(SizeU size) {
	DestroyFramebuffer();
	this->size = size;
	framebuffer = new Framebuffer(size);
}

void Layer::DestroyFramebuffer() {
	if (HasFramebuffer()) {
		delete static_cast<owner_ptr<Framebuffer>>(framebuffer);
		framebuffer = nullptr;
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	static_cast<ref_ptr<Framebuffer>>(GetFramebuffer())->Render(clip_region, [&](RenderTarget& target) { ViewDesign::RenderCanvas(target, canvas, offset, clip_region); });
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	static_cast<ref_ptr<Framebuffer>>(layer.GetFramebuffer())->Texture::TransitionImageLayout(target.CommandBuffer(), vk::ImageLayout::eShaderReadOnlyOptimal);


}


} // namespace ViewDesign

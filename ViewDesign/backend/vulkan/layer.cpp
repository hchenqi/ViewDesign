#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/vulkan/framebuffer.h"
#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

using namespace Vulkan;


void Layer::CreateFramebuffer(Size size) {
	DestroyFramebuffer();
	this->size = size;
	framebuffer = new Framebuffer(size);
}

void Layer::DestroyFramebuffer() {
	if (!IsEmpty()) {
		delete static_cast<owner_ptr<Framebuffer>>(framebuffer);
		framebuffer = nullptr;
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

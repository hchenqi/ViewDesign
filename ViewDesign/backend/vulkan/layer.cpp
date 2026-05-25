#include "ViewDesign/drawing/layer.h"

#include <ViewDesign/platform/vulkan/framebuffer.h>
#include <ViewDesign/platform/vulkan/pipeline/composite.h>


namespace ViewDesign {

using namespace Vulkan;


void Layer::Create(SizeU size) {
	Destroy();
	this->size = size;
	framebuffer = new Framebuffer(size);
}

void Layer::Destroy() {
	if (!Empty()) {
		DeviceContext::Get().device.waitIdle();
		delete static_cast<owner_ptr<Framebuffer>>(framebuffer);
		framebuffer = nullptr;
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	static_cast<ref_ptr<Framebuffer>>(GetFramebuffer())->Render(RectI(point_i_zero, size).Intersect(Round(clip_region)), [&](RenderTarget& target) { ViewDesign::RenderCanvas(target, canvas, offset, clip_region); });
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<CompositePipeline>();
	CompositePipeline::SetOpacity(target, opacity);
	CompositePipeline::BindTexture(target, *static_cast<ref_ptr<Framebuffer>>(layer.GetFramebuffer()));
	target.DrawVertices(Zip(GetVertices(AsQuad(Rect(point, size))), GetVertices(AsQuad(Normalize(layer.GetSize(), region)))));
}


} // namespace ViewDesign

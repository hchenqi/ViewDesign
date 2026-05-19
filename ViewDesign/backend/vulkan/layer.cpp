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
	target.SetOpacity(opacity);
	target.CommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *GetPipelineLayout<CompositePipeline::Layout>(), 0, *static_cast<ref_ptr<Framebuffer>>(layer.GetFramebuffer())->descriptor_set, {});
	target.DrawVertices(Zip(GetVertices(AsQuad(Rect(point, size))), GetVertices(AsQuad(Normalize(layer.GetSize(), region)))));
}


} // namespace ViewDesign

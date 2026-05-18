#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

using namespace Vulkan;


void Bitmap::CreateTexture() const {
	if (texture == nullptr) {
		texture = new Texture(pixel_buffer);
	}
}

void Bitmap::DestroyTexture() const {
	if (texture != nullptr) {
		delete static_cast<owner_ptr<Texture>>(texture);
		texture = nullptr;
	}
}


void BitmapFigure::DrawOn(RenderTarget& target, Point point) const {
	Texture& texture = *static_cast<ref_ptr<Texture>>(bitmap.GetTexture());
	texture.TransitionImageLayout(target.CommandBuffer(), vk::ImageLayout::eShaderReadOnlyOptimal);
	target.BindPipeline<CompositePipeline>();
	target.SetOpacity(opacity);
	target.CommandBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, GetPipelineLayout<CompositePipeline::Layout>(), 0, *texture.descriptor_set, {});
	target.DrawVertices(Zip(GetVertices(AsQuad(Rect(point, region.size))), GetVertices(AsQuad(Normalize(bitmap.GetSize(), region)))));
}


} // namespace ViewDesign

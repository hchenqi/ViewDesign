#include "ViewDesign/drawing/bitmap.h"

#include <ViewDesign/platform/vulkan/pipeline/composite.h>


namespace ViewDesign {

using namespace Vulkan;


void Bitmap::CreateTexture() const {
	if (texture == nullptr) {
		texture = new Texture(pixel_buffer);
	}
}

void Bitmap::DestroyTexture() const {
	if (texture != nullptr) {
		DeviceContext::Get().device.waitIdle();
		delete static_cast<owner_ptr<Texture>>(texture);
		texture = nullptr;
	}
}


void BitmapFigure::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<CompositePipeline>();
	CompositePipeline::SetOpacity(target, opacity);
	CompositePipeline::BindTexture(target, *static_cast<ref_ptr<Texture>>(bitmap.GetTexture()));
	target.DrawVertices(Zip(GetVertices(AsQuad(Rect(point, region.size))), GetVertices(AsQuad(Normalize(bitmap.GetSize(), region)))));
}


} // namespace ViewDesign

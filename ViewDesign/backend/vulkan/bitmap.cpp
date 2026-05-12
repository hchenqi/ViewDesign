#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/vulkan/texture.h"
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

}


} // namespace ViewDesign

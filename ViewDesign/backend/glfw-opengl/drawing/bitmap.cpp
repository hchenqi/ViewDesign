#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/glad/texture.h"


namespace ViewDesign {


const Texture& Bitmap::GetTexture() const {
	if (texture.IsEmpty()) {
		texture.Set(new TextureResource(pixel_buffer));
	}
	return texture;
}


void BitmapFigure::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

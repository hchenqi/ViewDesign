#include "ViewDesign/drawing/image.h"
#include "ViewDesign/platform/glad/texture.h"


namespace ViewDesign {


Image::Image(u16string file_name) : source(nullptr), size() {}

Image::Image(void* address, size_t size) : source(nullptr), size() {}

Image::~Image() {}

const Texture& Image::GetTexture() const {
	if (texture.IsEmpty()) {
		texture.Set(new TextureResource(PixelBuffer(0, 0)));
	}
	return texture;
}


void ImageFigure::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

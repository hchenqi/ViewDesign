#include "ViewDesign/view/figure/image.h"

#include <ViewDesign/platform/stb/image.h>

#include <ViewDesign/platform/glad/texture.h>
#include <ViewDesign/platform/glad/render_target.h>


namespace ViewDesign {

using namespace OpenGL;


namespace {

GLuint CreateTexture(const Stb::Image& image) {
	GLenum format;
	switch (image.channels) {
	case STBI_grey: format = GL_RED; break;
	case STBI_rgb: format = GL_RGB; break;
	case STBI_rgb_alpha: format = GL_RGBA; break;
	default: throw std::runtime_error("OpenGL: image format unsupported");
	}

	auto [width, height] = image.GetSize();
	GLuint texture = 0;
	glGenTextures(1, &texture);
	if (texture == 0) {
		throw std::runtime_error("OpenGL: texture creation failed");
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, image.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

} // namespace


Image::Image(const u16string& filename) : source(new Stb::Image(true, filename)), size(static_cast<ref_ptr<Stb::Image>>(source)->GetSize()) {}

Image::Image(const void* buffer, size_t size) : source(new Stb::Image(true, buffer, size)), size(static_cast<ref_ptr<Stb::Image>>(source)->GetSize()) {}

Image::~Image() { DestroyTexture(); delete static_cast<owner_ptr<Stb::Image>>(source); }

void Image::CreateTexture() const {
	if (texture == nullptr) {
		texture = new Texture(ViewDesign::CreateTexture(*static_cast<ref_ptr<Stb::Image>>(source)));
	}
}

void Image::DestroyTexture() const {
	if (texture != nullptr) {
		delete static_cast<owner_ptr<Texture>>(texture);
		texture = nullptr;
	}
}


void ImageFigure::DrawOn(RenderTarget& target, Point point) const {
	auto [dstX0, dstY0, dstX1, dstY1] = AsTuple(Rect(point, region.size));
	auto [srcU0, srcV0, srcU1, srcV1] = AsTupleNormalized(image.GetSize(), region);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, static_cast<ref_ptr<Texture>>(image.GetTexture())->GetId());

	glColor4f(opacity, opacity, opacity, opacity);

	glBegin(GL_QUADS);
	glTexCoord2f(srcU0, 1.0f - srcV0); glVertex2f(dstX0, dstY0);
	glTexCoord2f(srcU1, 1.0f - srcV0); glVertex2f(dstX1, dstY0);
	glTexCoord2f(srcU1, 1.0f - srcV1); glVertex2f(dstX1, dstY1);
	glTexCoord2f(srcU0, 1.0f - srcV1); glVertex2f(dstX0, dstY1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


} // namespace ViewDesign

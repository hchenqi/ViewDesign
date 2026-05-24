#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/platform/glad/render_target.h"


namespace ViewDesign {

using namespace OpenGL;


void Layer::Create(SizeU size) {
	Destroy();
	this->size = size;
	framebuffer = new Framebuffer(size);
}

void Layer::Destroy() {
	if (!Empty()) {
		delete static_cast<owner_ptr<Framebuffer>>(framebuffer);
		framebuffer = nullptr;
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	RenderContext context(size, static_cast<ref_ptr<Framebuffer>>(framebuffer));
	ViewDesign::RenderCanvas(static_cast<RenderTarget&>(context), canvas, offset, clip_region);
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	auto [dstX0, dstY0, dstX1, dstY1] = AsTuple(Rect(point, size));
	auto [srcU0, srcV0, srcU1, srcV1] = AsTupleNormalized(layer.GetSize(), region);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, static_cast<ref_ptr<Framebuffer>>(layer.GetFramebuffer())->Texture::GetId());

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

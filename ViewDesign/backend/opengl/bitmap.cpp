#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/glad/render_target.h"


namespace ViewDesign {

using namespace OpenGL;


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
	auto [dstX0, dstY0, dstX1, dstY1] = AsTuple(Rect(point, region.size));
	auto [srcU0, srcV0, srcU1, srcV1] = AsTupleNormalized(bitmap.GetSize(), region);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, static_cast<ref_ptr<Texture>>(bitmap.GetTexture())->GetId());

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

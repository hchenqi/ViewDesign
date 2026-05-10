#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/glad/render_target.h"


namespace ViewDesign {

using namespace OpenGL;


namespace {

inline Rect operator*(Rect rect, const Transform& transform) {
	Point a = rect.LeftTop() * transform;
	Point b = rect.RightTop() * transform;
	Point c = rect.LeftBottom() * transform;
	Point d = rect.RightBottom() * transform;
	float x_min = std::min({ a.x, b.x, c.x, d.x });
	float x_max = std::max({ a.x, b.x, c.x, d.x });
	float y_min = std::min({ a.y, b.y, c.y, d.y });
	float y_max = std::max({ a.y, b.y, c.y, d.y });
	return Rect(x_min, y_min, x_max - x_min, y_max - y_min);
}

} // namespace


void Layer::CreateTexture(Size size) {
	DestroyTexture();
	texture = new FrameBuffer(size);
}

void Layer::DestroyTexture() {
	if (!IsEmpty()) {
		delete static_cast<owner_ptr<FrameBuffer>>(texture);
	}
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	RenderContext context(size, static_cast<ref_ptr<FrameBuffer>>(texture));
	context.SetTransform(offset);
	context.PushAxisAlignedClip(clip_region + offset);
	context.Clear();
	auto& groups = canvas.GetFigureGroups();
	auto& figures = canvas.GetFigures();
	for (size_t figure_index = 0, group_index = 0; group_index < groups.size(); ++group_index) {
		auto& group = groups[group_index];
		for (; figure_index < group.figure_index; ++figure_index) {
			figures[figure_index].figure->DrawOn(render_target, figures[figure_index].offset);
		}
		Transform transform = group.transform * offset;
		if (group.IsBegin()) {
			context.SetTransform(transform);
			context.PushAxisAlignedClip(group.clip_region * transform);
		} else {
			context.PopAxisAlignedClip();
			context.SetTransform(transform);
		}
	}
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {
	auto [dstX0, dstY0, dstX1, dstY1] = AsOpenGLRect(Rect(point, size));
	auto [srcU0, srcV0, srcU1, srcV1] = AsOpenGLRectRatio(layer.GetSize(), region);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, static_cast<ref_ptr<FrameBuffer>>(layer.GetTexture())->Texture::GetId());

	glColor4f(1.0f, 1.0f, 1.0f, opacity);

	glBegin(GL_QUADS);
	glTexCoord2f(srcU0, srcV0); glVertex2f(dstX0, dstY0);
	glTexCoord2f(srcU1, srcV0); glVertex2f(dstX1, dstY0);
	glTexCoord2f(srcU1, srcV1); glVertex2f(dstX1, dstY1);
	glTexCoord2f(srcU0, srcV1); glVertex2f(dstX0, dstY1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


} // namespace ViewDesign

#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/glad/render_target.h"


namespace ViewDesign {

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


void Layer::Create(Size size) {
	this->size = size;
	texture.Set(new TextureResource(size));
}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {
	RenderTarget render_target(*texture.GetResource(), size);
	render_target.SetTransform(offset);
	render_target.PushAxisAlignedClip(clip_region + offset);
	render_target.Clear();
	auto& groups = canvas.GetFigureGroups();
	auto& figures = canvas.GetFigures();
	for (size_t figure_index = 0, group_index = 0; group_index < groups.size(); ++group_index) {
		auto& group = groups[group_index];
		for (; figure_index < group.figure_index; ++figure_index) {
			figures[figure_index].figure->DrawOn(render_target, figures[figure_index].offset);
		}
		Transform transform = group.transform * offset;
		if (group.IsBegin()) {
			render_target.SetTransform(transform);
			render_target.PushAxisAlignedClip(group.clip_region * transform);
		} else {
			render_target.PopAxisAlignedClip();
			render_target.SetTransform(transform);
		}
	}
}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

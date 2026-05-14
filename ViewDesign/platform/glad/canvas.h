#pragma once

#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/glad/render_target.h"


namespace ViewDesign {

namespace OpenGL {


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


inline void RenderCanvas(RenderTarget& target, const Canvas& canvas, Vector offset, Rect clip_region) {
	target.PushAxisAlignedClip(clip_region);
	target.Clear();
	target.SetTransform(offset);
	auto& figure_list = canvas.GetFigureList(); auto& group_list = canvas.GetGroupList();
	for (size_t figure_index = 0, group_index = 0; group_index < group_list.size(); ++group_index) {
		auto& [group_figure_index, group_transform, group_clip_region] = group_list[group_index];
		for (; figure_index < group_figure_index; ++figure_index) {
			auto& [point, figure] = figure_list[figure_index];
			figure->DrawOn(target, point);
		}
		Transform transform = group_transform * offset;
		if (!group_clip_region.IsEmpty()) {
			target.SetTransform(transform);
			target.PushAxisAlignedClip(group_clip_region * transform);
		} else {
			target.PopAxisAlignedClip();
			target.SetTransform(transform);
		}
	}
}


} // namespace OpenGL

} // namespace ViewDesign

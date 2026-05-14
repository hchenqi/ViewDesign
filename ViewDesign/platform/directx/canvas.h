#pragma once

#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/platform/directx/d2d_api.h"
#include "ViewDesign/platform/directx/helper.h"


namespace ViewDesign {

namespace DirectX {


inline void RenderCanvas(RenderTarget& target, const Canvas& canvas, Vector offset, Rect clip_region) {
	target.PushAxisAlignedClip(AsD2DRect(clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	target.Clear(AsD2DColor(color_transparent));
	target.SetTransform(AsD2DTransform(offset));
	auto& figure_list = canvas.GetFigureList(); auto& group_list = canvas.GetGroupList();
	for (size_t figure_index = 0, group_index = 0; group_index < group_list.size(); ++group_index) {
		auto& [group_figure_index, group_transform, group_clip_region] = group_list[group_index];
		for (; figure_index < group_figure_index; ++figure_index) {
			auto& [point, figure] = figure_list[figure_index];
			figure->DrawOn(target, point);
		}
		if (!group_clip_region.IsEmpty()) {
			target.SetTransform(AsD2DTransform(group_transform * offset));
			target.PushAxisAlignedClip(AsD2DRect(group_clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		} else {
			target.PopAxisAlignedClip();
			target.SetTransform(AsD2DTransform(group_transform * offset));
		}
	}
	target.SetTransform(AsD2DTransform(Transform::Identity()));
	target.PopAxisAlignedClip();
}


} // namespace DirectX

} // namespace ViewDesign

#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/drawing/figure.h"
#include "ViewDesign/geometry/transform.h"

#include <vector>
#include <memory>


namespace ViewDesign {


class Canvas {
	// offset
private:
	Vector offset = vector_zero;
public:
	void Offset(Vector offset, auto func) {
		this->offset += offset;
		func();
		this->offset -= offset;
	}

	// figure
private:
	std::vector<std::pair<Point, std::unique_ptr<const Figure>>> figure_list;
public:
	const auto& GetFigureList() const { return figure_list; }
public:
	void draw(Point offset, std::unique_ptr<const Figure> figure) {
		figure_list.emplace_back(offset + this->offset, std::move(figure));
	}
	void draw(Point offset, owner_ptr<const Figure> figure) {
		draw(offset, std::unique_ptr<const Figure>(figure));
	}
	template<class T> requires (std::derived_from<T, Figure>)
	void draw(Point offset, auto&&... args) {
		draw(offset, std::make_unique<T>(std::forward<decltype(args)>(args)...));
	}

	// transform
private:
	Transform current_transform;
public:
	const Transform& GetCurrentTransform() const { return current_transform; }

	// group
private:
	std::vector<std::tuple<size_t, Transform, Rect>> group_list;
public:
	const auto& GetGroupList() const { return group_list; }
public:
	void Group(Transform transform, Rect clip_region, auto func) {
		if (clip_region.IsEmpty()) { return; }
		Transform prev_transform = current_transform; Vector prev_offset = offset;
		current_transform = transform * offset * current_transform; offset = vector_zero;
		group_list.push_back(std::make_tuple(figure_list.size(), current_transform, clip_region));
		func();
		current_transform = prev_transform; offset = prev_offset;
		group_list.push_back(std::make_tuple(figure_list.size(), current_transform, rect_empty));
	}
};


template<class RenderTarget>
void RenderCanvas(RenderTarget& target, const Canvas& canvas, Vector offset, Rect clip_region) {
	target.PushClip(clip_region);
	target.Clear();
	target.SetTransform(offset);
	auto& figure_list = canvas.GetFigureList(); auto& group_list = canvas.GetGroupList();
	for (size_t figure_index = 0, group_index = 0; group_index < group_list.size(); ++group_index) {
		auto& [group_figure_index, group_transform, group_clip_region] = group_list[group_index];
		for (; figure_index < group_figure_index; ++figure_index) {
			auto& [point, figure] = figure_list[figure_index];
			figure->DrawOn(target, point);
		}
		if (!group_clip_region.IsEmpty()) {
			target.SetTransform(group_transform * offset);
			target.PushClip(group_clip_region);
		} else {
			target.PopClip();
			target.SetTransform(group_transform * offset);
		}
	}
	target.SetTransform(Transform::Identity());
	target.PopClip();
}


} // namespace ViewDesign

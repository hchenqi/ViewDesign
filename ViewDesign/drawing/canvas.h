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
	template<class T> requires std::derived_from<T, Figure>
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
	std::vector<std::tuple<uint, Transform, Rect>> group_list;
public:
	const auto& GetGroupList() const { return group_list; }
public:
	void Group(Transform transform, Rect clip_region, auto func) {
		if (clip_region.IsEmpty()) { return; }
		Transform prev_transform = current_transform; Vector prev_offset = offset;
		current_transform = transform * offset * current_transform; offset = vector_zero;
		group_list.push_back(std::make_tuple((uint)figure_list.size(), current_transform, clip_region));
		func();
		current_transform = prev_transform; offset = prev_offset;
		group_list.push_back(std::make_tuple((uint)figure_list.size(), current_transform, region_empty));
	}
};


} // namespace ViewDesign

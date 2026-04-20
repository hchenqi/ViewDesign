#pragma once

#include "figure.h"
#include "../geometry/transform.h"

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

	// transform
private:
	Transform transform;
public:
	const Transform& GetTransform() const { return transform; }

	// figure
private:
	struct FigureItem {
		Point offset;
		std::unique_ptr<const Figure> figure;
	};
	std::vector<FigureItem> figures;
public:
	const std::vector<FigureItem>& GetFigures() const { return figures; }
public:
	void draw(Point offset, std::unique_ptr<const Figure> figure) {
		figures.emplace_back(FigureItem{ offset + this->offset, std::move(figure) });
	}
	void draw(Point offset, owner_ptr<const Figure> figure) {
		draw(offset, std::unique_ptr<const Figure>(figure));
	}
	template<class FigureType>
	void draw(Point offset, auto&&... args) {
		draw(offset, std::make_unique<FigureType>(std::forward<decltype(args)>(args)...));
	}

	// group
private:
	struct FigureGroup {
		union {
			struct {  // as group begin
				uint figure_index;
				Transform transform;
				Rect clip_region;
			};
			struct {  // as group end
				uint figure_index;
				Transform prev_transform;
			};
		};
		bool IsBegin() const { return !clip_region.IsEmpty(); }
	};
	std::vector<FigureGroup> groups;
public:
	const std::vector<FigureGroup>& GetFigureGroups() const { return groups; }
public:
	void Group(Transform group_transform, Rect clip_region, auto func) {
		if (clip_region.IsEmpty()) { return; }
		Transform prev_transform = transform; Vector prev_offset = offset;
		transform = group_transform * offset * transform; offset = vector_zero;
		groups.push_back({ (uint)figures.size(), transform, clip_region });
		func();
		transform = prev_transform; offset = prev_offset;
		groups.push_back({ (uint)figures.size(), transform, region_empty });
	}

public:
	Canvas(auto func) {
		Group(Transform::Identity(), region_infinite, [&]() { func(*this); });
	}
};


} // namespace ViewDesign

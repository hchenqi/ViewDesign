#pragma once

#include "figure.h"
#include "../common/uncopyable.h"
#include "../geometry/transform.h"

#include <vector>
#include <memory>
#include <functional>


namespace ViewDesign {


class FigureQueue : public Uncopyable {
	// offset
private:
	Vector offset = vector_zero;
public:
	void Offset(Vector offset, std::function<void()> func) {
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
	void add(Point offset, std::unique_ptr<const Figure> figure) {
		figures.emplace_back(FigureItem{ offset + this->offset, std::move(figure) });
	}
	void add(Point offset, alloc_ptr<const Figure> figure) {
		add(offset, std::unique_ptr<const Figure>(figure));
	}
	template<class FigureType, class ...Types, class = std::enable_if_t<std::is_base_of_v<Figure, FigureType>>>
	void add(Point offset, Types&&... args) {
		add(offset, std::make_unique<FigureType>(std::forward<Types>(args)...));
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
	void Group(Transform group_transform, Rect clip_region, std::function<void()> func) {
		if (clip_region.IsEmpty()) { return; }
		Transform prev_transform = transform; Vector prev_offset = offset;
		transform = group_transform * offset * transform; offset = vector_zero;
		groups.push_back({ (uint)figures.size(), transform, clip_region });
		func();
		transform = prev_transform; offset = prev_offset;
		groups.push_back({ (uint)figures.size(), transform, region_empty });
	}

public:
	FigureQueue(std::function<void(FigureQueue&)> func) {
		Group(Transform::Identity(), region_infinite, [&]() { func(*this); });
	}
};


} // namespace ViewDesign

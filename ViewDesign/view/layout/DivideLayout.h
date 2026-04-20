#pragma once

#include "../view_traits.h"

#include <vector>


namespace ViewDesign {


template<class Direction>
class DivideLayout;


class _DivideLayout_Base : public ViewType<Fixed, Fixed> {
public:
	using child_type = view_ptr<Fixed, Fixed>;

protected:
	_DivideLayout_Base(std::vector<child_type> child_list) : child_list(std::move(child_list)) {
		size_t index = 0;
		for (auto& child : this->child_list) {
			RegisterChild(child);
			SetChildIndex(child, index++);
		}
	}

	// child
protected:
	std::vector<child_type> child_list;
protected:
	void SetChildIndex(ViewBase& child, size_t index) { ViewBase::SetChildData<size_t>(child, index); }
	size_t GetChildIndex(ViewBase& child) const { return ViewBase::GetChildData<size_t>(child); }

	// layout
protected:
	Size size;
	float child_length = 0.0f;
protected:
	size_t HitTestItem(float offset) { return size_t(offset / child_length); }
};


template<>
class DivideLayout<Vertical> : public _DivideLayout_Base {
public:
	DivideLayout(auto... child) : _DivideLayout_Base([&]() {
		std::vector<child_type> child_list; child_list.reserve(sizeof...(child));
		(child_list.emplace_back(std::move(child)), ...);
		return child_list;
	}()) {}

	// layout
protected:
	Rect GetChildRegion(size_t index) const { return Rect(Point(0.0f, index * child_length), Size(size.width, child_length)); }
	Rect GetChildRegion(ViewBase& child) const { return GetChildRegion(GetChildIndex(child)); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return GetChildRegion(child).point - point_zero;
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size != size_ref) {
			size = size_ref;
			if (child_list.size() > 0) {
				child_length = size.height / child_list.size();
				for (auto& child : child_list) {
					UpdateChildSizeRef(child, Size(size.width, child_length));
				}
			}
		}
		return size;
	}

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		if (child_list.empty()) { return; }
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		size_t begin = HitTestItem(draw_region.top()), end = HitTestItem(ceilf(draw_region.bottom()) - 1.0f);
		for (size_t index = begin; index <= end; ++index) { DrawChild(child_list[index], GetChildRegion(index), canvas, draw_region); }
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		float offset = event.point.y;
		if (offset < 0.0f || offset >= size.height || child_list.empty()) { return nullptr; }
		size_t index = HitTestItem(offset); event.point.y -= index * child_length;
		return child_list[index];
	}
};


template<>
class DivideLayout<Horizontal> : public _DivideLayout_Base {
public:
	DivideLayout(auto... child) : _DivideLayout_Base([&]() {
		std::vector<child_type> child_list; child_list.reserve(sizeof...(child));
		(child_list.emplace_back(std::move(child)), ...);
		return child_list;
	}()) {}

	// layout
protected:
	Rect GetChildRegion(size_t index) const { return Rect(Point(index * child_length, 0.0f), Size(child_length, size.height)); }
	Rect GetChildRegion(ViewBase& child) const { return GetChildRegion(GetChildIndex(child)); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return GetChildRegion(child).point - point_zero;
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size != size_ref) {
			size = size_ref;
			if (child_list.size() > 0) {
				child_length = size.width / child_list.size();
				for (auto& child : child_list) {
					UpdateChildSizeRef(child, Size(child_length, size.height));
				}
			}
		}
		return size;
	}

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		if (child_list.empty()) { return; }
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		size_t begin = HitTestItem(draw_region.left()), end = HitTestItem(ceilf(draw_region.right()) - 1.0f);
		for (size_t index = begin; index <= end; ++index) { DrawChild(child_list[index], GetChildRegion(index), canvas, draw_region); }
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		float offset = event.point.x;
		if (offset < 0.0f || offset >= size.width || child_list.empty()) { return nullptr; }
		size_t index = HitTestItem(offset); event.point.x -= index * child_length;
		return child_list[index];
	}
};


} // namespace ViewDesign

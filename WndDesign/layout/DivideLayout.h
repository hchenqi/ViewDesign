#pragma once

#include "../window/wnd_traits.h"

#include <vector>


namespace ViewDesign {


template<class Direction>
class DivideLayout;


class _DivideLayout_Base : public WndType<Assigned, Assigned> {
public:
	using child_type = child_ptr<Assigned, Assigned>;

protected:
	_DivideLayout_Base() : child_list() {}

	// child
protected:
	std::vector<child_type> child_list;
protected:
	void SetChildIndex(WndObject& child, size_t index) { WndObject::SetChildData<size_t>(child, index); }
	size_t GetChildIndex(WndObject& child) const { return WndObject::GetChildData<size_t>(child); }

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
	template<class... Ts>
	DivideLayout(Ts... child_args) : _DivideLayout_Base() {
		child_list.reserve(sizeof...(Ts)); (child_list.emplace_back(std::move(child_args)), ...);
		uint index = 0;	for (auto& child : child_list) { RegisterChild(child); SetChildIndex(child, index++); }
	}

	// layout
protected:
	Rect GetChildRegion(size_t index) const { return Rect(Point(0.0f, index * child_length), Size(size.width, child_length)); }
	Rect GetChildRegion(WndObject& child) const { return GetChildRegion(GetChildIndex(child)); }
protected:
	virtual Transform GetChildTransform(WndObject& child) const override {
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

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		if (child_list.empty()) { return; }
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		size_t begin = HitTestItem(draw_region.top()), end = HitTestItem(ceilf(draw_region.bottom()) - 1.0f);
		for (size_t index = begin; index <= end; ++index) { DrawChild(child_list[index], GetChildRegion(index), figure_queue, draw_region); }
	}

	// message
protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		float offset = msg.point.y;
		if (offset < 0.0f || offset >= size.height || child_list.empty()) { return nullptr; }
		size_t index = HitTestItem(offset); msg.point.y -= index * child_length;
		return child_list[index];
	}
};


template<>
class DivideLayout<Horizontal> : public _DivideLayout_Base {
public:
	template<class... Ts>
	DivideLayout(Ts... child_args) : _DivideLayout_Base() {
		child_list.reserve(sizeof...(Ts)); (child_list.emplace_back(std::move(child_args)), ...);
		uint index = 0;	for (auto& child : child_list) { RegisterChild(child); SetChildIndex(child, index++); }
	}

	// layout
protected:
	Rect GetChildRegion(size_t index) const { return Rect(Point(index * child_length, 0.0f), Size(child_length, size.height)); }
	Rect GetChildRegion(WndObject& child) const { return GetChildRegion(GetChildIndex(child)); }
protected:
	virtual Transform GetChildTransform(WndObject& child) const override {
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

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		if (child_list.empty()) { return; }
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		size_t begin = HitTestItem(draw_region.left()), end = HitTestItem(ceilf(draw_region.right()) - 1.0f);
		for (size_t index = begin; index <= end; ++index) { DrawChild(child_list[index], GetChildRegion(index), figure_queue, draw_region); }
	}

	// message
protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override {
		float offset = msg.point.x;
		if (offset < 0.0f || offset >= size.width || child_list.empty()) { return nullptr; }
		size_t index = HitTestItem(offset); msg.point.x -= index * child_length;
		return child_list[index];
	}
};


} // namespace ViewDesign

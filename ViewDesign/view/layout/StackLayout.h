#pragma once

#include "../view_traits.h"
#include "../../common/reversion_wrapper.h"


namespace ViewDesign {


class _StackLayout_Base : public ViewBase {
protected:
	_StackLayout_Base(view_ptr<> child_first, view_ptr<> child_second) : child_first(std::move(child_first)), child_second(std::move(child_second)) {
		RegisterChild(this->child_first); RegisterChild(this->child_second);
	}

	// child
protected:
	view_ptr<> child_first;
	view_ptr<> child_second;

	// layout
protected:
	Size size;
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		DrawChild(child_first, point_zero, figure_queue, draw_region);
		DrawChild(child_second, point_zero, figure_queue, draw_region);
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		if (ref_ptr<ViewBase> res = HitTestChild(child_second, event)) { return res; }
		return HitTestChild(child_first, event = event_copy);
	}
};


template<class WidthTypeFirst, class HeightTypeFirst, class WidthTypeSecond, class HeightTypeSecond>
class StackLayout : public _StackLayout_Base {
public:
	static_assert((IsAssigned<WidthTypeFirst>&& IsAssigned<HeightTypeFirst>) || (IsAssigned<WidthTypeSecond> && IsAssigned<HeightTypeSecond>), "At least one child window's width type and height type should be Assigned.");
public:
	using width_type = std::conditional_t<IsAssigned<WidthTypeFirst>, WidthTypeSecond, WidthTypeFirst>;
	using height_type = std::conditional_t<IsAssigned<HeightTypeFirst>, HeightTypeSecond, HeightTypeFirst>;
public:
	using child_type_first = view_ptr<WidthTypeFirst, HeightTypeFirst>;
	using child_type_second = view_ptr<WidthTypeSecond, HeightTypeSecond>;

public:
	StackLayout(child_type_first child_first, child_type_second child_second) : _StackLayout_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = size_ref;

		if constexpr (!IsAssigned<WidthTypeFirst> || !IsAssigned<HeightTypeFirst>) {
			size = UpdateChildSizeRef(child_first, size);
		}
		if constexpr (!IsAssigned<WidthTypeSecond> || !IsAssigned<HeightTypeSecond>) {
			size = UpdateChildSizeRef(child_second, size);
		}

		if constexpr (IsAssigned<WidthTypeFirst> && IsAssigned<HeightTypeFirst>) {
			UpdateChildSizeRef(child_first, size);
		}
		if constexpr (IsAssigned<WidthTypeSecond> && IsAssigned<HeightTypeSecond>) {
			UpdateChildSizeRef(child_second, size);
		}

		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (&child == child_first.get()) {
			if constexpr (!IsAssigned<WidthTypeFirst> || !IsAssigned<HeightTypeFirst>) {
				size = child_size;
				UpdateChildSizeRef(child_second, size);
				SizeUpdated(size);
			}
		} else {
			if constexpr (!IsAssigned<WidthTypeSecond> || !IsAssigned<HeightTypeSecond>) {
				size = child_size;
				UpdateChildSizeRef(child_first, size);
				SizeUpdated(size);
			}
		}
	}
};


template<class T1, class T2>
StackLayout(T1, T2) -> StackLayout<extract_width_type<T1>, extract_height_type<T1>, extract_width_type<T2>, extract_height_type<T2>>;


class StackLayoutMultiple : public ViewType<Assigned, Assigned> {
public:
	using child_type = view_ptr<Assigned, Assigned>;

public:
	template<class... Ts>
	StackLayoutMultiple(Ts... child_args) : child_list() {
		child_list.reserve(sizeof...(Ts)); (child_list.emplace_back(std::move(child_args)), ...);
		for (auto& child : child_list) { RegisterChild(child); }
	}

	// child
protected:
	std::vector<child_type> child_list;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { for (auto& child : child_list) { UpdateChildSizeRef(child, size_ref); } return size_ref; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override { for (auto& child : child_list) { DrawChild(child, point_zero, figure_queue, draw_region); } }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		for (auto& child : reverse(child_list)) {
			if (ref_ptr<ViewBase> res = HitTestChild(child, event)) {
				return res;
			}
			event = event_copy;
		}
		return nullptr;
	}
};


} // namespace ViewDesign

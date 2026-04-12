#pragma once

#include "view_traits.h"
#include "../common/tuple_helper.h"


namespace ViewDesign {


class ViewFrame : public ViewBase {
public:
	ViewFrame(view_ptr<> child) : child(std::move(child)) { RegisterChild(this->child); }
	virtual ~ViewFrame() override {}

	// child
protected:
	view_ptr<> child;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { DrawChild(child, point_zero, canvas, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


class ViewFrameRef : public ViewBase {
public:
	ViewFrameRef(view_ref<> child) : child(child) { RegisterChild(child); }
	virtual ~ViewFrameRef() override { UnregisterChild(child); }

	// child
protected:
	view_ref<> child;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { DrawChild(child, point_zero, canvas, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


class ViewFrameMutable : public ViewFrame {
public:
	ViewFrameMutable(view_ptr<> child) : ViewFrame(std::move(child)) {}

	// child
public:
	template<size_t K = 1, class... T> requires ((std::is_same_v<ViewFrameMutable, T> || std::is_base_of_v<view_ptr<>, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("ViewFrameMutable::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameMutable&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> view_ptr<>& { if constexpr (std::is_same_v<ViewFrameMutable&, decltype(other)>) { return other.child; } else { return other; }};
		auto view_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto view_ptr_tuple_rotated = move_rotate_tuple<K>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(view_ptr_tuple_rotated), view_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameMutable&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	view_ptr<> Reset(view_ptr<> child) { Rotate(*this, child); return child; }
	void SwapWith(ViewFrameMutable& other) { Rotate(*this, other); }

	// layout
private:
	Size size_ref;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		return ViewFrame::OnSizeRefUpdate(this->size_ref = size_ref);
	}
};


class ViewFrameRefMutable : public ViewFrameRef {
public:
	ViewFrameRefMutable(view_ref<> child) : ViewFrameRef(child) {}

	// child
public:
	template<size_t K = 1, class... T> requires ((std::is_same_v<ViewFrameRefMutable, T> || std::is_base_of_v<view_ref<>, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("ViewFrameMutable::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameRefMutable&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> view_ref<>& { if constexpr (std::is_same_v<ViewFrameRefMutable&, decltype(other)>) { return other.child; } else { return other; }};
		auto view_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto view_ptr_tuple_rotated = move_rotate_tuple<K>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(view_ptr_tuple_rotated), view_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameRefMutable&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	view_ref<> Reset(view_ref<> child) { Rotate(*this, child); return child; }
	void SwapWith(ViewFrameRefMutable& other) { Rotate(*this, other); }

	// layout
private:
	Size size_ref;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		return ViewFrameRef::OnSizeRefUpdate(this->size_ref = size_ref);
	}
};


} // namespace ViewDesign

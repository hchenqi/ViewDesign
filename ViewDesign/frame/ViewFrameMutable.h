#pragma once

#include "ViewFrame.h"
#include "../common/tuple_helper.h"


namespace ViewDesign {


class ViewFrameMutable : public ViewFrame {
public:
	ViewFrameMutable(child_ptr<> child) : ViewFrame(std::move(child)) {}

	// child
public:
	template<size_t K = 1, class... T> requires ((std::is_same_v<ViewFrameMutable, T> || std::is_base_of_v<child_ptr<>, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("ViewFrameMutable::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameMutable&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> child_ptr<>& { if constexpr (std::is_same_v<ViewFrameMutable&, decltype(other)>) { return other.child; } else { return other; }};
		auto child_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto child_ptr_tuple_rotated = move_rotate_tuple<K>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(child_ptr_tuple_rotated), child_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameMutable&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	child_ptr<> Reset(child_ptr<> child) { Rotate(*this, child); return child; }
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
	ViewFrameRefMutable(child_ref<> child) : ViewFrameRef(child) {}

	// child
public:
	template<size_t K = 1, class... T> requires ((std::is_same_v<ViewFrameRefMutable, T> || std::is_base_of_v<child_ref<>, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("ViewFrameMutable::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameRefMutable&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> child_ref<>& { if constexpr (std::is_same_v<ViewFrameRefMutable&, decltype(other)>) { return other.child; } else { return other; }};
		auto child_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto child_ptr_tuple_rotated = move_rotate_tuple<K>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(child_ptr_tuple_rotated), child_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::is_same_v<ViewFrameRefMutable&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	child_ref<> Reset(child_ref<> child) { Rotate(*this, child); return child; }
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

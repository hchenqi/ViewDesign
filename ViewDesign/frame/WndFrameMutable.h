#pragma once

#include "WndFrame.h"
#include "../common/tuple_helper.h"


namespace ViewDesign {


class WndFrameMutable : public WndFrame {
public:
	WndFrameMutable(child_ptr<> child) : WndFrame(std::move(child)) {}

	// child
public:
	template<size_t K = 1, class... T> requires ((std::is_same_v<WndFrameMutable, T> || std::is_base_of_v<child_ptr<>, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("WndFrameMutable::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::is_same_v<WndFrameMutable&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> child_ptr<>& { if constexpr (std::is_same_v<WndFrameMutable&, decltype(other)>) { return other.child; } else { return other; }};
		auto child_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto child_ptr_tuple_rotated = move_rotate_tuple<K>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(child_ptr_tuple_rotated), child_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::is_same_v<WndFrameMutable&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	child_ptr<> Reset(child_ptr<> child) { Rotate(*this, child); return child; }
	void SwapWith(WndFrameMutable& other) { Rotate(*this, other); }

	// layout
private:
	Size size_ref;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		return WndFrame::OnSizeRefUpdate(this->size_ref = size_ref);
	}
};


class WndFrameRefMutable : public WndFrameRef {
public:
	WndFrameRefMutable(child_ref<> child) : WndFrameRef(child) {}

	// child
public:
	template<size_t K = 1, class... T> requires ((std::is_same_v<WndFrameRefMutable, T> || std::is_base_of_v<child_ref<>, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("WndFrameMutable::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::is_same_v<WndFrameRefMutable&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> child_ref<>& { if constexpr (std::is_same_v<WndFrameRefMutable&, decltype(other)>) { return other.child; } else { return other; }};
		auto child_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto child_ptr_tuple_rotated = move_rotate_tuple<K>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(child_ptr_tuple_rotated), child_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::is_same_v<WndFrameRefMutable&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	child_ref<> Reset(child_ref<> child) { Rotate(*this, child); return child; }
	void SwapWith(WndFrameRefMutable& other) { Rotate(*this, other); }

	// layout
private:
	Size size_ref;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		return WndFrameRef::OnSizeRefUpdate(this->size_ref = size_ref);
	}
};


} // namespace ViewDesign

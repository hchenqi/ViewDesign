#include "../ViewFrame.h"
#include "../../common/tuple_helper.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class MutableFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	MutableFrame(child_type child) : ViewFrame(std::move(child)) {}

	// child
public:
	template<class... T> requires ((std::same_as<MutableFrame, T> || std::same_as<child_type, T>) && ...)
	static void Rotate(T&... other) {
		if constexpr (sizeof...(other) <= 1) {
			throw std::invalid_argument("MutableFrame::Rotate: there should be at least 2 arguments");
		}

		([](auto& other) {
			if constexpr (std::same_as<MutableFrame&, decltype(other)>) {
				other.UnregisterChild(other.child);
			}
		}(other), ...);

		auto map_to_child = [](auto& other) -> view_ptr<>& { if constexpr (std::same_as<MutableFrame&, decltype(other)>) { return other.child; } else { return other; }};
		auto view_ptr_ref_tuple = std::tie(map_to_child(other)...);
		auto view_ptr_tuple_rotated = move_rotate_tuple<1>(std::make_tuple((std::move(map_to_child(other)))...));
		move_assign_tuple(std::move(view_ptr_tuple_rotated), view_ptr_ref_tuple);

		([](auto& other) {
			if constexpr (std::same_as<MutableFrame&, decltype(other)>) {
				other.RegisterChild(other.child);
				other.SizeUpdated(other.UpdateChildSizeRef(other.child, other.size_ref));
				other.Redraw(region_infinite);
			}
		}(other), ...);
	}
public:
	child_type Reset(child_type child) { Rotate(*this, child); return child; }
	void SwapWith(MutableFrame& other) { Rotate(*this, other); }
	void RotateWith(auto&... other) { Rotate(*this, other...); }

	// layout
private:
	Size size_ref;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		return ViewFrame::OnSizeRefUpdate(this->size_ref = size_ref);
	}
};


template<class T>
MutableFrame(T) -> MutableFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

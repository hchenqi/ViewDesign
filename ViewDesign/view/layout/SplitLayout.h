#pragma once

#include "../view_traits.h"


namespace ViewDesign {


class _SplitLayout_Base : public ViewBase {
protected:
	using child_type = view_ptr<>;

protected:
	_SplitLayout_Base(child_type child_first, child_type child_second) : child_first(std::move(child_first)), child_second(std::move(child_second)) {
		RegisterChild(this->child_first); RegisterChild(this->child_second);
	}

	// child
protected:
	child_type child_first;
	child_type child_second;
protected:
	bool IsFirst(ViewBase& child) const { return &child == child_first.get(); }

	// layout
protected:
	Size size_ref;
	Size size;
	float length_first = 0.0f;
	float length_second = 0.0f;
};


class _SplitLayout_Base_Vertical : public _SplitLayout_Base {
protected:
	_SplitLayout_Base_Vertical(child_type child_first, child_type child_second) : _SplitLayout_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	Rect GetRegionFirst() const { return Rect(point_zero, Size(size.width, length_first)); }
	Rect GetRegionSecond() const { return Rect(Point(0.0f, length_first), Size(size.width, length_second)); }
	Rect GetChildRegion(ViewBase& child) const { return IsFirst(child) ? GetRegionFirst() : GetRegionSecond(); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return GetChildRegion(child).point - point_zero;
	}

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		DrawChild(child_first, GetRegionFirst(), canvas, draw_region);
		DrawChild(child_second, GetRegionSecond(), canvas, draw_region);
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.point.y < length_first) { return child_first; }
		event.point.y -= length_first;
		if (event.point.y < length_second) { return child_second; }
		return nullptr;
	}
};

class _SplitLayout_Base_Horizontal : public _SplitLayout_Base {
protected:
	_SplitLayout_Base_Horizontal(child_type child_first, child_type child_second) : _SplitLayout_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	Rect GetRegionFirst() const { return Rect(point_zero, Size(length_first, size.height)); }
	Rect GetRegionSecond() const { return Rect(Point(length_first, 0.0f), Size(length_second, size.height)); }
	Rect GetChildRegion(ViewBase& child) const { return IsFirst(child) ? GetRegionFirst() : GetRegionSecond(); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return GetChildRegion(child).point - point_zero;
	}

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		DrawChild(child_first, GetRegionFirst(), canvas, draw_region);
		DrawChild(child_second, GetRegionSecond(), canvas, draw_region);
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.point.x < length_first) { return child_first; }
		event.point.x -= length_first;
		if (event.point.x < length_second) { return child_second; }
		return nullptr;
	}
};


template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
class SplitLayoutVertical : public _SplitLayout_Base_Vertical {
public:
	static_assert(IsFixed<WidthTraitFirst> || IsFixed<WidthTraitSecond>, "At least one child window's width type should be Fixed.");
	static_assert(!IsFixed<HeightTraitFirst> || !IsFixed<HeightTraitSecond>, "At least one child window's height type should not be Fixed.");
	static_assert((IsFixed<WidthTraitFirst> || !IsFixed<HeightTraitFirst>) && (IsFixed<WidthTraitSecond> || !IsFixed<HeightTraitSecond>), "Child window's width type must be Fixed if its height type is Fixed.");
public:
	using width_trait = std::conditional_t<IsFixed<WidthTraitFirst> && IsFixed<WidthTraitSecond>, Fixed, std::conditional_t<IsRelative<WidthTraitFirst> || IsRelative<WidthTraitSecond>, Relative, Auto>>;
	using height_trait = std::conditional_t<IsFixed<HeightTraitFirst> || IsFixed<HeightTraitSecond>, Fixed, std::conditional_t<IsRelative<HeightTraitFirst> || IsRelative<HeightTraitSecond>, Relative, Auto>>;
public:
	using child_type_first = view_ptr<WidthTraitFirst, HeightTraitFirst>;
	using child_type_second = view_ptr<WidthTraitSecond, HeightTraitSecond>;

public:
	SplitLayoutVertical(child_type_first child_first, child_type_second child_second) : _SplitLayout_Base_Vertical(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;

		if constexpr (!IsFixed<WidthTraitFirst> || (IsFixed<width_trait> && !IsFixed<HeightTraitFirst>)) {
			Size child_size = UpdateChildSizeRef(child_first, size_ref);
			if constexpr (!IsFixed<WidthTraitFirst>) {
				size.width = child_size.width;
			}
			if constexpr (!IsFixed<HeightTraitFirst>) {
				length_first = child_size.height;
			}
		}
		if constexpr (!IsFixed<WidthTraitSecond> || (IsFixed<width_trait> && !IsFixed<HeightTraitSecond>)) {
			Size child_size = UpdateChildSizeRef(child_second, size_ref);
			if constexpr (!IsFixed<WidthTraitSecond>) {
				size.width = child_size.width;
			}
			if constexpr (!IsFixed<HeightTraitSecond>) {
				length_second = child_size.height;
			}
		}
		if constexpr (IsFixed<width_trait>) {
			size.width = size_ref.width;
		}

		if constexpr (IsFixed<height_trait>) {
			size.height = size_ref.height;
		}
		if constexpr (IsFixed<WidthTraitFirst> && (!IsFixed<width_trait> || IsFixed<HeightTraitFirst>)) {
			if constexpr (IsFixed<HeightTraitFirst>) {
				UpdateChildSizeRef(child_first, Size(size.width, length_first = size.height - length_second));
			} else {
				length_first = UpdateChildSizeRef(child_first, Size(size.width, size_ref.height)).height;
			}
		}
		if constexpr (IsFixed<WidthTraitSecond> && (!IsFixed<width_trait> || IsFixed<HeightTraitSecond>)) {
			if constexpr (IsFixed<HeightTraitSecond>) {
				UpdateChildSizeRef(child_second, Size(size.width, length_second = size.height - length_first));
			} else {
				length_second = UpdateChildSizeRef(child_second, Size(size.width, size_ref.height)).height;
			}
		}
		if constexpr (!IsFixed<height_trait>) {
			size.height = length_first + length_second;
		}

		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (IsFirst(child)) {
			if constexpr (!IsFixed<HeightTraitFirst>) {
				if (IsFixed<WidthTraitFirst> || size.width == child_size.width) {
					if (length_first != child_size.height) {
						length_first = child_size.height;
						if constexpr (IsFixed<HeightTraitSecond>) {
							UpdateChildSizeRef(child_second, Size(size.width, length_second = size.height - length_first));
							Redraw(region_infinite);
						} else {
							size.height = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.width = child_size.width;
					length_first = child_size.height;
					if constexpr (IsFixed<HeightTraitSecond>) {
						UpdateChildSizeRef(child_second, Size(size.width, length_second = size.height - length_first));
					} else {
						length_second = UpdateChildSizeRef(child_second, Size(size.width, size_ref.height)).height;
						size.height = length_first + length_second;
					}
					SizeUpdated(size);
				}
			}
		} else {
			if constexpr (!IsFixed<HeightTraitSecond>) {
				if (IsFixed<WidthTraitSecond> || size.width == child_size.width) {
					if (length_second != child_size.height) {
						length_second = child_size.height;
						if constexpr (IsFixed<HeightTraitFirst>) {
							UpdateChildSizeRef(child_first, Size(size.width, length_first = size.height - length_second));
							Redraw(region_infinite);
						} else {
							size.height = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.width = child_size.width;
					length_second = child_size.height;
					if constexpr (IsFixed<HeightTraitFirst>) {
						UpdateChildSizeRef(child_first, Size(size.width, length_first = size.height - length_second));
					} else {
						length_second = UpdateChildSizeRef(child_first, Size(size.width, size_ref.height)).height;
						size.height = length_first + length_second;
					}
					SizeUpdated(size);
				}
			}
		}
	}
};

template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
class SplitLayoutHorizontal : public _SplitLayout_Base_Horizontal {
public:
	static_assert(!IsFixed<WidthTraitFirst> || !IsFixed<WidthTraitSecond>, "At least one child window's width type should not be Fixed.");
	static_assert(IsFixed<HeightTraitFirst> || IsFixed<HeightTraitSecond>, "At least one child window's height type should be Fixed.");
	static_assert((!IsFixed<WidthTraitFirst> || IsFixed<HeightTraitFirst>) && (!IsFixed<WidthTraitSecond> || IsFixed<HeightTraitSecond>), "Child window's height type must be Fixed if its width type is Fixed.");
public:
	using width_trait = std::conditional_t<IsFixed<WidthTraitFirst> || IsFixed<WidthTraitSecond>, Fixed, std::conditional_t<IsRelative<WidthTraitFirst> || IsRelative<WidthTraitSecond>, Relative, Auto>>;
	using height_trait = std::conditional_t<IsFixed<HeightTraitFirst> && IsFixed<HeightTraitSecond>, Fixed, std::conditional_t<IsRelative<HeightTraitFirst> || IsRelative<HeightTraitSecond>, Relative, Auto>>;
public:
	using child_type_first = view_ptr<WidthTraitFirst, HeightTraitFirst>;
	using child_type_second = view_ptr<WidthTraitSecond, HeightTraitSecond>;

public:
	SplitLayoutHorizontal(child_type_first child_first, child_type_second child_second) : _SplitLayout_Base_Horizontal(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;

		if constexpr (!IsFixed<HeightTraitFirst> || (IsFixed<height_trait> && !IsFixed<WidthTraitFirst>)) {
			Size child_size = UpdateChildSizeRef(child_first, size_ref);
			if constexpr (!IsFixed<HeightTraitFirst>) {
				size.height = child_size.height;
			}
			if constexpr (!IsFixed<WidthTraitFirst>) {
				length_first = child_size.width;
			}
		}
		if constexpr (!IsFixed<HeightTraitSecond> || (IsFixed<height_trait> && !IsFixed<WidthTraitSecond>)) {
			Size child_size = UpdateChildSizeRef(child_second, size_ref);
			if constexpr (!IsFixed<HeightTraitSecond>) {
				size.height = child_size.height;
			}
			if constexpr (!IsFixed<WidthTraitSecond>) {
				length_second = child_size.width;
			}
		}
		if constexpr (IsFixed<height_trait>) {
			size.height = size_ref.height;
		}

		if constexpr (IsFixed<width_trait>) {
			size.width = size_ref.width;
		}
		if constexpr (IsFixed<HeightTraitFirst> && (!IsFixed<height_trait> || IsFixed<WidthTraitFirst>)) {
			if constexpr (IsFixed<WidthTraitFirst>) {
				UpdateChildSizeRef(child_first, Size(length_first = size.width - length_second, size.height));
			} else {
				length_first = UpdateChildSizeRef(child_first, Size(size_ref.width, size.height)).width;
			}
		}
		if constexpr (IsFixed<HeightTraitSecond> && (!IsFixed<height_trait> || IsFixed<WidthTraitSecond>)) {
			if constexpr (IsFixed<WidthTraitSecond>) {
				UpdateChildSizeRef(child_second, Size(length_second = size.width - length_first, size.height));
			} else {
				length_second = UpdateChildSizeRef(child_second, Size(size_ref.width, size.height)).width;
			}
		}
		if constexpr (!IsFixed<height_trait>) {
			size.width = length_first + length_second;
		}

		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (IsFirst(child)) {
			if constexpr (!IsFixed<WidthTraitFirst>) {
				if (IsFixed<HeightTraitFirst> || size.height == child_size.height) {
					if (length_first != child_size.width) {
						length_first = child_size.width;
						if constexpr (IsFixed<WidthTraitSecond>) {
							UpdateChildSizeRef(child_second, Size(length_second = size.width - length_first, size.height));
							Redraw(region_infinite);
						} else {
							size.width = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.height = child_size.height;
					length_first = child_size.width;
					if constexpr (IsFixed<WidthTraitSecond>) {
						UpdateChildSizeRef(child_second, Size(length_second = size.width - length_first, size.height));
					} else {
						length_second = UpdateChildSizeRef(child_second, Size(size_ref.width, size.height)).width;
						size.width = length_first + length_second;
					}
					SizeUpdated(size);
				}
			}
		} else {
			if constexpr (!IsFixed<WidthTraitSecond>) {
				if (IsFixed<HeightTraitSecond> || size.height == child_size.height) {
					if (length_second != child_size.width) {
						length_second = child_size.width;
						if constexpr (IsFixed<WidthTraitFirst>) {
							UpdateChildSizeRef(child_first, Size(length_first = size.width - length_second, size.height));
							Redraw(region_infinite);
						} else {
							size.width = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.height = child_size.height;
					length_second = child_size.width;
					if constexpr (IsFixed<WidthTraitFirst>) {
						UpdateChildSizeRef(child_first, Size(length_first = size.width - length_second, size.height));
					} else {
						length_second = UpdateChildSizeRef(child_first, Size(size_ref.width, size.height)).width;
						size.width = length_first + length_second;
					}
					SizeUpdated(size);
				}
			}
		}
	}
};


template<class T1, class T2>
SplitLayoutVertical(T1, T2) -> SplitLayoutVertical<extract_width_trait<T1>, extract_height_trait<T1>, extract_width_trait<T2>, extract_height_trait<T2>>;

template<class T1, class T2>
SplitLayoutHorizontal(T1, T2) -> SplitLayoutHorizontal<extract_width_trait<T1>, extract_height_trait<T1>, extract_width_trait<T2>, extract_height_trait<T2>>;


} // namespace ViewDesign

#pragma once

#include "../window/wnd_traits.h"


namespace ViewDesign {


class _SplitLayout_Base : public WndObject {
protected:
	using child_type = child_ptr<>;

protected:
	_SplitLayout_Base(child_type child_first, child_type child_second) : child_first(std::move(child_first)), child_second(std::move(child_second)) {
		RegisterChild(this->child_first); RegisterChild(this->child_second);
	}

	// child
protected:
	child_type child_first;
	child_type child_second;
protected:
	bool IsFirst(WndObject& child) const { return &child == child_first.get(); }

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
	Rect GetChildRegion(WndObject& child) const { return IsFirst(child) ? GetRegionFirst() : GetRegionSecond(); }
protected:
	virtual Transform GetChildTransform(WndObject& child) const override {
		return GetChildRegion(child).point - point_zero;
	}

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		DrawChild(child_first, GetRegionFirst(), figure_queue, draw_region);
		DrawChild(child_second, GetRegionSecond(), figure_queue, draw_region);
	}

	// event
protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
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
	Rect GetChildRegion(WndObject& child) const { return IsFirst(child) ? GetRegionFirst() : GetRegionSecond(); }
protected:
	virtual Transform GetChildTransform(WndObject& child) const override {
		return GetChildRegion(child).point - point_zero;
	}

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		DrawChild(child_first, GetRegionFirst(), figure_queue, draw_region);
		DrawChild(child_second, GetRegionSecond(), figure_queue, draw_region);
	}

	// event
protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override {
		if (event.point.x < length_first) { return child_first; }
		event.point.x -= length_first;
		if (event.point.x < length_second) { return child_second; }
		return nullptr;
	}
};


template<class WidthTypeFirst, class HeightTypeFirst, class WidthTypeSecond, class HeightTypeSecond>
class SplitLayoutVertical : public _SplitLayout_Base_Vertical {
public:
	static_assert(IsAssigned<WidthTypeFirst> || IsAssigned<WidthTypeSecond>, "At least one child window's width type should be Assigned.");
	static_assert(!IsAssigned<HeightTypeFirst> || !IsAssigned<HeightTypeSecond>, "At least one child window's height type should not be Assigned.");
	static_assert((IsAssigned<WidthTypeFirst> || !IsAssigned<HeightTypeFirst>) && (IsAssigned<WidthTypeSecond> || !IsAssigned<HeightTypeSecond>), "Child window's width type must be Assigned if its height type is Assigned.");
public:
	using width_type = std::conditional_t<IsAssigned<WidthTypeFirst> && IsAssigned<WidthTypeSecond>, Assigned, std::conditional_t<IsRelative<WidthTypeFirst> || IsRelative<WidthTypeSecond>, Relative, Auto>>;
	using height_type = std::conditional_t<IsAssigned<HeightTypeFirst> || IsAssigned<HeightTypeSecond>, Assigned, std::conditional_t<IsRelative<HeightTypeFirst> || IsRelative<HeightTypeSecond>, Relative, Auto>>;
public:
	using child_type_first = child_ptr<WidthTypeFirst, HeightTypeFirst>;
	using child_type_second = child_ptr<WidthTypeSecond, HeightTypeSecond>;

public:
	SplitLayoutVertical(child_type_first child_first, child_type_second child_second) : _SplitLayout_Base_Vertical(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;

		if constexpr (!IsAssigned<WidthTypeFirst> || (IsAssigned<width_type> && !IsAssigned<HeightTypeFirst>)) {
			Size child_size = UpdateChildSizeRef(child_first, size_ref);
			if constexpr (!IsAssigned<WidthTypeFirst>) {
				size.width = child_size.width;
			}
			if constexpr (!IsAssigned<HeightTypeFirst>) {
				length_first = child_size.height;
			}
		}
		if constexpr (!IsAssigned<WidthTypeSecond> || (IsAssigned<width_type> && !IsAssigned<HeightTypeSecond>)) {
			Size child_size = UpdateChildSizeRef(child_second, size_ref);
			if constexpr (!IsAssigned<WidthTypeSecond>) {
				size.width = child_size.width;
			}
			if constexpr (!IsAssigned<HeightTypeSecond>) {
				length_second = child_size.height;
			}
		}
		if constexpr (IsAssigned<width_type>) {
			size.width = size_ref.width;
		}

		if constexpr (IsAssigned<height_type>) {
			size.height = size_ref.height;
		}
		if constexpr (IsAssigned<WidthTypeFirst> && (!IsAssigned<width_type> || IsAssigned<HeightTypeFirst>)) {
			if constexpr (IsAssigned<HeightTypeFirst>) {
				UpdateChildSizeRef(child_first, Size(size.width, length_first = size.height - length_second));
			} else {
				length_first = UpdateChildSizeRef(child_first, Size(size.width, size_ref.height)).height;
			}
		}
		if constexpr (IsAssigned<WidthTypeSecond> && (!IsAssigned<width_type> || IsAssigned<HeightTypeSecond>)) {
			if constexpr (IsAssigned<HeightTypeSecond>) {
				UpdateChildSizeRef(child_second, Size(size.width, length_second = size.height - length_first));
			} else {
				length_second = UpdateChildSizeRef(child_second, Size(size.width, size_ref.height)).height;
			}
		}
		if constexpr (!IsAssigned<height_type>) {
			size.height = length_first + length_second;
		}

		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		if (IsFirst(child)) {
			if constexpr (!IsAssigned<HeightTypeFirst>) {
				if (IsAssigned<WidthTypeFirst> || size.width == child_size.width) {
					if (length_first != child_size.height) {
						length_first = child_size.height;
						if constexpr (IsAssigned<HeightTypeSecond>) {
							UpdateChildSizeRef(child_second, Size(size.width, length_second = size.height - length_first));
							Redraw(GetRegionSecond());
						} else {
							size.height = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.width = child_size.width;
					length_first = child_size.height;
					if constexpr (IsAssigned<HeightTypeSecond>) {
						UpdateChildSizeRef(child_second, Size(size.width, length_second = size.height - length_first));
					} else {
						length_second = UpdateChildSizeRef(child_second, Size(size.width, size_ref.height)).height;
						size.height = length_first + length_second;
					}
					SizeUpdated(size);
				}
			}
		} else {
			if constexpr (!IsAssigned<HeightTypeSecond>) {
				if (IsAssigned<WidthTypeSecond> || size.width == child_size.width) {
					if (length_second != child_size.height) {
						length_second = child_size.height;
						if constexpr (IsAssigned<HeightTypeFirst>) {
							UpdateChildSizeRef(child_first, Size(size.width, length_first = size.height - length_second));
							Redraw(GetRegionFirst());
						} else {
							size.height = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.width = child_size.width;
					length_second = child_size.height;
					if constexpr (IsAssigned<HeightTypeFirst>) {
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

template<class WidthTypeFirst, class HeightTypeFirst, class WidthTypeSecond, class HeightTypeSecond>
class SplitLayoutHorizontal : public _SplitLayout_Base_Horizontal {
public:
	static_assert(!IsAssigned<WidthTypeFirst> || !IsAssigned<WidthTypeSecond>, "At least one child window's width type should not be Assigned.");
	static_assert(IsAssigned<HeightTypeFirst> || IsAssigned<HeightTypeSecond>, "At least one child window's height type should be Assigned.");
	static_assert((!IsAssigned<WidthTypeFirst> || IsAssigned<HeightTypeFirst>) && (!IsAssigned<WidthTypeSecond> || IsAssigned<HeightTypeSecond>), "Child window's height type must be Assigned if its width type is Assigned.");
public:
	using width_type = std::conditional_t<IsAssigned<WidthTypeFirst> || IsAssigned<WidthTypeSecond>, Assigned, std::conditional_t<IsRelative<WidthTypeFirst> || IsRelative<WidthTypeSecond>, Relative, Auto>>;
	using height_type = std::conditional_t<IsAssigned<HeightTypeFirst> && IsAssigned<HeightTypeSecond>, Assigned, std::conditional_t<IsRelative<HeightTypeFirst> || IsRelative<HeightTypeSecond>, Relative, Auto>>;
public:
	using child_type_first = child_ptr<WidthTypeFirst, HeightTypeFirst>;
	using child_type_second = child_ptr<WidthTypeSecond, HeightTypeSecond>;

public:
	SplitLayoutHorizontal(child_type_first child_first, child_type_second child_second) : _SplitLayout_Base_Horizontal(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;

		if constexpr (!IsAssigned<HeightTypeFirst> || (IsAssigned<height_type> && !IsAssigned<WidthTypeFirst>)) {
			Size child_size = UpdateChildSizeRef(child_first, size_ref);
			if constexpr (!IsAssigned<HeightTypeFirst>) {
				size.height = child_size.height;
			}
			if constexpr (!IsAssigned<WidthTypeFirst>) {
				length_first = child_size.width;
			}
		}
		if constexpr (!IsAssigned<HeightTypeSecond> || (IsAssigned<height_type> && !IsAssigned<WidthTypeSecond>)) {
			Size child_size = UpdateChildSizeRef(child_second, size_ref);
			if constexpr (!IsAssigned<HeightTypeSecond>) {
				size.height = child_size.height;
			}
			if constexpr (!IsAssigned<WidthTypeSecond>) {
				length_second = child_size.width;
			}
		}
		if constexpr (IsAssigned<height_type>) {
			size.height = size_ref.height;
		}

		if constexpr (IsAssigned<width_type>) {
			size.width = size_ref.width;
		}
		if constexpr (IsAssigned<HeightTypeFirst> && (!IsAssigned<height_type> || IsAssigned<WidthTypeFirst>)) {
			if constexpr (IsAssigned<WidthTypeFirst>) {
				UpdateChildSizeRef(child_first, Size(length_first = size.width - length_second, size.height));
			} else {
				length_first = UpdateChildSizeRef(child_first, Size(size_ref.width, size.height)).width;
			}
		}
		if constexpr (IsAssigned<HeightTypeSecond> && (!IsAssigned<height_type> || IsAssigned<WidthTypeSecond>)) {
			if constexpr (IsAssigned<WidthTypeSecond>) {
				UpdateChildSizeRef(child_second, Size(length_second = size.width - length_first, size.height));
			} else {
				length_second = UpdateChildSizeRef(child_second, Size(size_ref.width, size.height)).width;
			}
		}
		if constexpr (!IsAssigned<height_type>) {
			size.width = length_first + length_second;
		}

		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		if (IsFirst(child)) {
			if constexpr (!IsAssigned<WidthTypeFirst>) {
				if (IsAssigned<HeightTypeFirst> || size.height == child_size.height) {
					if (length_first != child_size.width) {
						length_first = child_size.width;
						if constexpr (IsAssigned<WidthTypeSecond>) {
							UpdateChildSizeRef(child_second, Size(length_second = size.width - length_first, size.height));
							Redraw(GetRegionSecond());
						} else {
							size.width = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.height = child_size.height;
					length_first = child_size.width;
					if constexpr (IsAssigned<WidthTypeSecond>) {
						UpdateChildSizeRef(child_second, Size(length_second = size.width - length_first, size.height));
					} else {
						length_second = UpdateChildSizeRef(child_second, Size(size_ref.width, size.height)).width;
						size.width = length_first + length_second;
					}
					SizeUpdated(size);
				}
			}
		} else {
			if constexpr (!IsAssigned<WidthTypeSecond>) {
				if (IsAssigned<HeightTypeSecond> || size.height == child_size.height) {
					if (length_second != child_size.width) {
						length_second = child_size.width;
						if constexpr (IsAssigned<WidthTypeFirst>) {
							UpdateChildSizeRef(child_first, Size(length_first = size.width - length_second, size.height));
							Redraw(GetRegionFirst());
						} else {
							size.width = length_first + length_second;
							SizeUpdated(size);
						}
					}
				} else {
					size.height = child_size.height;
					length_second = child_size.width;
					if constexpr (IsAssigned<WidthTypeFirst>) {
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
SplitLayoutVertical(T1, T2) -> SplitLayoutVertical<extract_width_type<T1>, extract_height_type<T1>, extract_width_type<T2>, extract_height_type<T2>>;

template<class T1, class T2>
SplitLayoutHorizontal(T1, T2) -> SplitLayoutHorizontal<extract_width_type<T1>, extract_height_type<T1>, extract_width_type<T2>, extract_height_type<T2>>;


} // namespace ViewDesign

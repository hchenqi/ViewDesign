#pragma once

#include "ViewDesign/view/view_traits.h"


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
	Size size_first;
	Size size_second;
};


class _SplitLayoutVertical_Base : public _SplitLayout_Base {
protected:
	_SplitLayoutVertical_Base(child_type child_first, child_type child_second) : _SplitLayout_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	Rect GetRegionFirst() const { return Rect(point_zero, Size(size.width, size_first.height)); }
	Rect GetRegionSecond() const { return Rect(Point(0.0f, size_first.height), Size(size.width, size_second.height)); }
	Rect GetChildRegion(ViewBase& child) const { return IsFirst(child) ? GetRegionFirst() : GetRegionSecond(); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return GetChildRegion(child).point - point_zero;
	}

	// drawing
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
		if (event.point.y < size_first.height) { return child_first; }
		event.point.y -= size_first.height;
		if (event.point.y < size_second.height) { return child_second; }
		return nullptr;
	}
};

class _SplitLayoutHorizontal_Base : public _SplitLayout_Base {
protected:
	_SplitLayoutHorizontal_Base(child_type child_first, child_type child_second) : _SplitLayout_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	Rect GetRegionFirst() const { return Rect(point_zero, Size(size_first.width, size.height)); }
	Rect GetRegionSecond() const { return Rect(Point(size_first.width, 0.0f), Size(size_second.width, size.height)); }
	Rect GetChildRegion(ViewBase& child) const { return IsFirst(child) ? GetRegionFirst() : GetRegionSecond(); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return GetChildRegion(child).point - point_zero;
	}

	// drawing
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
		if (event.point.x < size_first.width) { return child_first; }
		event.point.x -= size_first.width;
		if (event.point.x < size_second.width) { return child_second; }
		return nullptr;
	}
};


template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
class SplitLayoutVertical : public _SplitLayoutVertical_Base {
public:
	static_assert(!IsFixed<HeightTraitFirst> || !IsFixed<HeightTraitSecond>, "At least one child view's height should not be Fixed.");
	static_assert((!IsFixed<HeightTraitFirst> || IsFixed<WidthTraitFirst> || !IsFixed<WidthTraitSecond>) && (!IsFixed<HeightTraitSecond> || IsFixed<WidthTraitSecond> || !IsFixed<WidthTraitFirst>), "If one child view's height is Fixed and width is not Fixed, the other child view's width must not be Fixed");
public:
	using width_trait = std::conditional_t<IsFixed<WidthTraitFirst> && IsFixed<WidthTraitSecond>, Fixed, std::conditional_t<IsRelative<WidthTraitFirst> || IsRelative<WidthTraitSecond>, Relative, Auto>>;
	using height_trait = std::conditional_t<IsFixed<HeightTraitFirst> || IsFixed<HeightTraitSecond>, Fixed, std::conditional_t<IsRelative<HeightTraitFirst> || IsRelative<HeightTraitSecond>, Relative, Auto>>;
public:
	using child_type_first = view_ptr<WidthTraitFirst, HeightTraitFirst>;
	using child_type_second = view_ptr<WidthTraitSecond, HeightTraitSecond>;

public:
	SplitLayoutVertical(child_type_first child_first, child_type_second child_second) : _SplitLayoutVertical_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;

		if constexpr (IsFixed<width_trait>) {
			size.width = size_ref.width;
		}
		if constexpr (IsFixed<height_trait>) {
			size.height = size_ref.height;
		}

		if constexpr (!IsFixed<HeightTraitFirst> && (!IsFixed<WidthTraitFirst> || IsFixed<WidthTraitSecond>)) {
			size_first = UpdateChildSizeRef(child_first, size_ref);
		}
		if constexpr (!IsFixed<HeightTraitSecond> && (!IsFixed<WidthTraitSecond> || IsFixed<WidthTraitFirst>)) {
			size_second = UpdateChildSizeRef(child_second, size_ref);
		}

		if constexpr (!IsFixed<WidthTraitFirst> && IsFixed<WidthTraitSecond>) {
			size.width = size_first.width;
		}
		if constexpr (IsFixed<WidthTraitFirst> && !IsFixed<WidthTraitSecond>) {
			size.width = size_second.width;
		}

		if constexpr (IsFixed<HeightTraitFirst> || (IsFixed<WidthTraitFirst> && !IsFixed<WidthTraitSecond>)) {
			size_first = UpdateChildSizeRef(child_first, Size(IsFixed<WidthTraitFirst> ? size.width : size_ref.width, IsFixed<HeightTraitFirst> ? size.height - size_second.height : size_ref.height));
		}
		if constexpr (IsFixed<HeightTraitSecond> || (IsFixed<WidthTraitSecond> && !IsFixed<WidthTraitFirst>)) {
			size_second = UpdateChildSizeRef(child_second, Size(IsFixed<WidthTraitSecond> ? size.width : size_ref.width, IsFixed<HeightTraitSecond> ? size.height - size_first.height : size_ref.height));
		}

		if constexpr (!IsFixed<WidthTraitFirst> && !IsFixed<WidthTraitSecond>) {
			size.width = std::max(size_first.width, size_second.width);
		}
		if constexpr (!IsFixed<height_trait>) {
			size.height = size_first.height + size_second.height;
		}

		return size;
	}
private:
	void UpdateWidthOrRedraw(Rect redraw_region) {
		float width = std::max(size_first.width, size_second.width);
		if (size.width != width) {
			size.width = width;
			SizeUpdated(size);
		} else {
			Redraw(redraw_region);
		}
	}
	void UpdateHeight() {
		size.height = size_first.height + size_second.height;
		SizeUpdated(size);
	}
protected:
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (IsFirst(child)) {
			if constexpr (!IsFixed<HeightTraitFirst> || !IsFixed<WidthTraitFirst>) {
				if (IsFixed<WidthTraitFirst> || size_first.width == child_size.width) {
					if (!IsFixed<HeightTraitFirst> && size_first.height != child_size.height) {
						size_first.height = child_size.height;
						if constexpr (IsFixed<HeightTraitSecond>) {
							if constexpr (IsFixed<WidthTraitSecond>) {
								size_second = UpdateChildSizeRef(child_second, Size(size.width, size.height - size_first.height));
								Redraw(region_infinite);
							} else {
								size_second = UpdateChildSizeRef(child_second, Size(size_ref.width, size.height - size_first.height));
								UpdateWidthOrRedraw(region_infinite);
							}
						} else {
							UpdateHeight();
						}
					}
				} else {
					size_first.width = child_size.width;
					if constexpr (IsFixed<WidthTraitSecond>) {
						size.width = size_first.width;
						size_first.height = child_size.height;
						if constexpr (IsFixed<HeightTraitSecond>) {
							size_second = UpdateChildSizeRef(child_second, Size(size.width, size.height - size_first.height));
							SizeUpdated(size);
						} else {
							size_second = UpdateChildSizeRef(child_second, Size(size.width, size_ref.height));
							UpdateHeight();
						}
					} else {
						if constexpr (IsFixed<HeightTraitSecond>) {
							if (size_first.height != child_size.height) {
								size_first.height = child_size.height;
								size_second = UpdateChildSizeRef(child_second, Size(size_ref.width, size.height - size_first.height));
								UpdateWidthOrRedraw(region_infinite);
							} else {
								UpdateWidthOrRedraw(GetRegionFirst());
							}
						} else {
							if (!IsFixed<HeightTraitFirst> && size_first.height != child_size.height) {
								size_first.height = child_size.height;
								size.width = std::max(size_first.width, size_second.width);
								UpdateHeight();
							} else {
								UpdateWidthOrRedraw(GetRegionFirst());
							}
						}
					}
				}
			}
		} else {
			if constexpr (!IsFixed<HeightTraitSecond> || !IsFixed<WidthTraitSecond>) {
				if (IsFixed<WidthTraitSecond> || size_second.width == child_size.width) {
					if (!IsFixed<HeightTraitSecond> && size_second.height != child_size.height) {
						size_second.height = child_size.height;
						if constexpr (IsFixed<HeightTraitFirst>) {
							if constexpr (IsFixed<WidthTraitFirst>) {
								size_first = UpdateChildSizeRef(child_first, Size(size.width, size.height - size_second.height));
								Redraw(region_infinite);
							} else {
								size_first = UpdateChildSizeRef(child_first, Size(size_ref.width, size.height - size_second.height));
								UpdateWidthOrRedraw(region_infinite);
							}
						} else {
							UpdateHeight();
						}
					}
				} else {
					size_second.width = child_size.width;
					if constexpr (IsFixed<WidthTraitFirst>) {
						size.width = size_second.width;
						size_second.height = child_size.height;
						if constexpr (IsFixed<HeightTraitFirst>) {
							size_first = UpdateChildSizeRef(child_first, Size(size.width, size.height - size_second.height));
							SizeUpdated(size);
						} else {
							size_first = UpdateChildSizeRef(child_first, Size(size.width, size_ref.height));
							UpdateHeight();
						}
					} else {
						if constexpr (IsFixed<HeightTraitFirst>) {
							if (size_second.height != child_size.height) {
								size_second.height = child_size.height;
								size_first = UpdateChildSizeRef(child_first, Size(size_ref.width, size.height - size_second.height));
								UpdateWidthOrRedraw(region_infinite);
							} else {
								UpdateWidthOrRedraw(GetRegionSecond());
							}
						} else {
							if (!IsFixed<HeightTraitSecond> && size_second.height != child_size.height) {
								size_second.height = child_size.height;
								size.width = std::max(size_first.width, size_second.width);
								UpdateHeight();
							} else {
								UpdateWidthOrRedraw(GetRegionSecond());
							}
						}
					}
				}
			}
		}
	}
};

template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
class SplitLayoutHorizontal : public _SplitLayoutHorizontal_Base {
public:
	static_assert(!IsFixed<WidthTraitFirst> || !IsFixed<WidthTraitSecond>, "At least one child view's width should not be Fixed.");
	static_assert((!IsFixed<WidthTraitFirst> || IsFixed<HeightTraitFirst> || !IsFixed<HeightTraitSecond>) && (!IsFixed<WidthTraitSecond> || IsFixed<HeightTraitSecond> || !IsFixed<HeightTraitFirst>), "If one child view's width is Fixed and height is not Fixed, the other child view's height must not be Fixed");
public:
	using width_trait = std::conditional_t<IsFixed<WidthTraitFirst> || IsFixed<WidthTraitSecond>, Fixed, std::conditional_t<IsRelative<WidthTraitFirst> || IsRelative<WidthTraitSecond>, Relative, Auto>>;
	using height_trait = std::conditional_t<IsFixed<HeightTraitFirst> && IsFixed<HeightTraitSecond>, Fixed, std::conditional_t<IsRelative<HeightTraitFirst> || IsRelative<HeightTraitSecond>, Relative, Auto>>;
public:
	using child_type_first = view_ptr<WidthTraitFirst, HeightTraitFirst>;
	using child_type_second = view_ptr<WidthTraitSecond, HeightTraitSecond>;

public:
	SplitLayoutHorizontal(child_type_first child_first, child_type_second child_second) : _SplitLayoutHorizontal_Base(std::move(child_first), std::move(child_second)) {}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		this->size_ref = size_ref;

		if constexpr (IsFixed<height_trait>) {
			size.height = size_ref.height;
		}
		if constexpr (IsFixed<width_trait>) {
			size.width = size_ref.width;
		}

		if constexpr (!IsFixed<WidthTraitFirst> && (!IsFixed<HeightTraitFirst> || IsFixed<HeightTraitSecond>)) {
			size_first = UpdateChildSizeRef(child_first, size_ref);
		}
		if constexpr (!IsFixed<WidthTraitSecond> && (!IsFixed<HeightTraitSecond> || IsFixed<HeightTraitFirst>)) {
			size_second = UpdateChildSizeRef(child_second, size_ref);
		}

		if constexpr (!IsFixed<HeightTraitFirst> && IsFixed<HeightTraitSecond>) {
			size.height = size_first.height;
		}
		if constexpr (IsFixed<HeightTraitFirst> && !IsFixed<HeightTraitSecond>) {
			size.height = size_second.height;
		}

		if constexpr (IsFixed<WidthTraitFirst> || (IsFixed<HeightTraitFirst> && !IsFixed<HeightTraitSecond>)) {
			size_first = UpdateChildSizeRef(child_first, Size(IsFixed<WidthTraitFirst> ? size.width - size_second.width : size_ref.width, IsFixed<HeightTraitFirst> ? size.height : size_ref.height));
		}
		if constexpr (IsFixed<WidthTraitSecond> || (IsFixed<HeightTraitSecond> && !IsFixed<HeightTraitFirst>)) {
			size_second = UpdateChildSizeRef(child_second, Size(IsFixed<WidthTraitSecond> ? size.width - size_first.width : size_ref.width, IsFixed<HeightTraitSecond> ? size.height : size_ref.height));
		}

		if constexpr (!IsFixed<HeightTraitFirst> && !IsFixed<HeightTraitSecond>) {
			size.height = std::max(size_first.height, size_second.height);
		}
		if constexpr (!IsFixed<width_trait>) {
			size.width = size_first.width + size_second.width;
		}

		return size;
	}
private:
	void UpdateHeightOrRedraw(Rect redraw_region) {
		float height = std::max(size_first.height, size_second.height);
		if (size.height != height) {
			size.height = height;
			SizeUpdated(size);
		} else {
			Redraw(redraw_region);
		}
	}
	void UpdateWidth() {
		size.width = size_first.width + size_second.width;
		SizeUpdated(size);
	}
protected:
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (IsFirst(child)) {
			if constexpr (!IsFixed<WidthTraitFirst> || !IsFixed<HeightTraitFirst>) {
				if (IsFixed<HeightTraitFirst> || size_first.height == child_size.height) {
					if (!IsFixed<WidthTraitFirst> && size_first.width != child_size.width) {
						size_first.width = child_size.width;
						if constexpr (IsFixed<WidthTraitSecond>) {
							if constexpr (IsFixed<HeightTraitSecond>) {
								size_second = UpdateChildSizeRef(child_second, Size(size.width - size_first.width, size.height));
								Redraw(region_infinite);
							} else {
								size_second = UpdateChildSizeRef(child_second, Size(size.width - size_first.width, size_ref.height));
								UpdateHeightOrRedraw(region_infinite);
							}
						} else {
							UpdateWidth();
						}
					}
				} else {
					size_first.height = child_size.height;
					if constexpr (IsFixed<HeightTraitSecond>) {
						size.height = size_first.height;
						size_first.width = child_size.width;
						if constexpr (IsFixed<WidthTraitSecond>) {
							size_second = UpdateChildSizeRef(child_second, Size(size.width - size_first.width, size.height));
							SizeUpdated(size);
						} else {
							size_second = UpdateChildSizeRef(child_second, Size(size_ref.height, size.height));
							UpdateWidth();
						}
					} else {
						if constexpr (IsFixed<WidthTraitSecond>) {
							if (size_first.width != child_size.width) {
								size_first.width = child_size.width;
								size_second = UpdateChildSizeRef(child_second, Size(size.width - size_first.width, size_ref.height));
								UpdateHeightOrRedraw(region_infinite);
							} else {
								UpdateHeightOrRedraw(GetRegionFirst());
							}
						} else {
							if (!IsFixed<WidthTraitFirst> && size_first.width != child_size.width) {
								size_first.width = child_size.width;
								size.height = std::max(size_first.height, size_second.height);
								UpdateWidth();
							} else {
								UpdateHeightOrRedraw(GetRegionFirst());
							}
						}
					}
				}
			}
		} else {
			if constexpr (!IsFixed<WidthTraitSecond> || !IsFixed<HeightTraitSecond>) {
				if (IsFixed<HeightTraitSecond> || size_second.height == child_size.height) {
					if (!IsFixed<WidthTraitSecond> && size_second.width != child_size.width) {
						size_second.width = child_size.width;
						if constexpr (IsFixed<WidthTraitFirst>) {
							if constexpr (IsFixed<HeightTraitFirst>) {
								size_first = UpdateChildSizeRef(child_first, Size(size.width - size_second.width, size.height));
								Redraw(region_infinite);
							} else {
								size_first = UpdateChildSizeRef(child_first, Size(size.width - size_second.width, size_ref.height));
								UpdateHeightOrRedraw(region_infinite);
							}
						} else {
							UpdateWidth();
						}
					}
				} else {
					size_second.height = child_size.height;
					if constexpr (IsFixed<HeightTraitFirst>) {
						size.height = size_second.height;
						size_second.width = child_size.width;
						if constexpr (IsFixed<WidthTraitFirst>) {
							size_first = UpdateChildSizeRef(child_first, Size(size.width - size_second.width, size.height));
							SizeUpdated(size);
						} else {
							size_first = UpdateChildSizeRef(child_first, Size(size_ref.height, size.height));
							UpdateWidth();
						}
					} else {
						if constexpr (IsFixed<WidthTraitFirst>) {
							if (size_second.width != child_size.width) {
								size_second.width = child_size.width;
								size_first = UpdateChildSizeRef(child_first, Size(size.width - size_second.width, size_ref.height));
								UpdateHeightOrRedraw(region_infinite);
							} else {
								UpdateHeightOrRedraw(GetRegionSecond());
							}
						} else {
							if (!IsFixed<WidthTraitSecond> && size_second.width != child_size.width) {
								size_second.width = child_size.width;
								size.height = std::max(size_first.height, size_second.height);
								UpdateWidth();
							} else {
								UpdateHeightOrRedraw(GetRegionSecond());
							}
						}
					}
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

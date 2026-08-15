#pragma once

#include "ViewDesign/view/view_traits.h"


namespace ViewDesign {

template<class View>
class DeferredReflow;


class _ListLayout_Base : public ViewBase {
protected:
	struct ChildInfo {
	public:
		view_ptr_any view;
		Rect region = rect_empty;
	public:
		ChildInfo(view_ptr_any view) : view(std::move(view)) {}
	};

protected:
	_ListLayout_Base(float gap, std::vector<ChildInfo> child_list) : gap(gap), child_list(std::move(child_list)) {
		for (auto& child : this->child_list) { RegisterChild(child.view); }
	}

	// style
protected:
	float gap;

	// child
protected:
	std::vector<ChildInfo> child_list;
public:
	bool Empty() const { return child_list.empty(); }
	size_t Length() const { return child_list.size(); }
	ViewBase& GetChild(size_t index) const { return child_list[index].view; }
	size_t GetChildIndex(ViewBase& child) const {
		auto it = std::find_if(child_list.begin(), child_list.end(), [&](const ChildInfo& info) { return info.view == &child; });
		if (it == child_list.end()) { throw std::invalid_argument("invalid child"); }
		return it - child_list.begin();
	}
protected:
	std::vector<ChildInfo>::iterator NormalizeInsertIndex(size_t index) {
		if (index > child_list.size()) { index = child_list.size(); }
		return child_list.begin() + index;
	}
	std::vector<ChildInfo>::iterator NormalizeEraseIndex(size_t index) {
		if (index >= child_list.size()) { throw std::invalid_argument("invalid index"); }
		return child_list.begin() + index;
	}
	std::pair<std::vector<ChildInfo>::iterator, std::vector<ChildInfo>::iterator> NormalizeEraseIndex(size_t index, size_t count) {
		if (index >= child_list.size() || count == 0) { throw std::invalid_argument("invalid index"); }
		auto begin = child_list.begin() + index, end = begin + std::min(count, child_list.size() - index);
		return std::make_pair(begin, end);
	}
protected:
	std::vector<ChildInfo>::iterator Insert(std::vector<ChildInfo>::iterator it, view_ptr_any child) {
		return child_list.emplace(it, std::move(child));
	}
	std::pair<std::vector<ChildInfo>::iterator, std::vector<ChildInfo>::iterator> Insert(std::vector<ChildInfo>::iterator it, auto list) requires (std::ranges::input_range<decltype(list)>) {
		auto begin = child_list.insert(it, std::make_move_iterator(list.begin()), std::make_move_iterator(list.end())), end = begin + list.size();
		return std::make_pair(begin, end);
	}
	std::pair<ChildInfo, std::vector<ChildInfo>::iterator> Erase(std::vector<ChildInfo>::iterator it) {
		ChildInfo child = std::move(*it);
		it = child_list.erase(it);
		return std::make_pair(std::move(child), it);
	}
	std::pair<std::vector<ChildInfo>, std::vector<ChildInfo>::iterator> Erase(std::vector<ChildInfo>::iterator begin, std::vector<ChildInfo>::iterator end) {
		std::vector<ChildInfo> list(std::make_move_iterator(begin), std::make_move_iterator(end));
		auto it = child_list.erase(begin, end);
		return std::make_pair(std::move(list), it);
	}

	// layout
protected:
	Size size;
protected:
	Rect GetChildRegion(ViewBase& child) const { return child_list[GetChildIndex(child)].region; }
protected:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const override { return point + (GetChildRegion(child).point - point_zero); }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const override { return point - (GetChildRegion(child).point - point_zero); }

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Rect child_region = GetChildRegion(child);
		Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
	}
};


class _ListLayoutVertical_Base : public _ListLayout_Base {
protected:
	using _ListLayout_Base::_ListLayout_Base;

	// layout
protected:
	float width_ref = 0.0f;
protected:
	std::vector<ChildInfo>::const_iterator HitTestIndex(float y) {
		return std::lower_bound(child_list.begin(), child_list.end(), y, [](const ChildInfo& child, float y) { return child.region.point.y <= y; }) - 1;
	}
protected:
	float UpdateOffset(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator end) {
		float y = it == child_list.begin() ? 0.0f : (it - 1)->region.bottom() + gap;
		for (; it != end; ++it) {
			it->region.point.y = y;
			y += it->region.size.height + gap;
		}
		for (; it != child_list.end(); ++it) {
			if (it->region.point.y == y) {
				return size.height;
			}
			it->region.point.y = y;
			y += it->region.size.height + gap;
		}
		return child_list.empty() ? y : y - gap;
	}
	float UpdateOffset(std::vector<ChildInfo>::iterator it) {
		return UpdateOffset(it, it);
	}
	void UpdateHeight(float height) {
		if (size.height != height) {
			size.height = height;
			SizeUpdated(size);
		}
	}
	void UpdateOffsetHeight(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator end) {
		UpdateHeight(UpdateOffset(it, end));
	}
	void UpdateOffsetHeight(std::vector<ChildInfo>::iterator it) {
		UpdateOffsetHeight(it, it);
	}

	// drawing
protected:
	void RedrawChild(std::vector<ChildInfo>::const_iterator it) {
		Redraw(Rect(Point(0, it->region.top()), Size(length_infinite, it->region.height())));
	}
	void RedrawChild(std::vector<ChildInfo>::const_iterator first, std::vector<ChildInfo>::const_iterator last) {
		Redraw(Rect(Point(0, first->region.top()), Size(length_infinite, last->region.bottom() - first->region.top())));
	}
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		auto begin = HitTestIndex(draw_region.top()), end = HitTestIndex(draw_region.bottom());
		for (auto it = begin; it <= end; ++it) {
			DrawChild(it->view, it->region, canvas, draw_region);
		}
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.point.y < 0.0f || event.point.y >= size.height) { return nullptr; }
		auto child = HitTestIndex(event.point.y);
		if (event.point.y >= child->region.bottom()) { return this; }
		event.point.y -= child->region.point.y;
		return child->view;
	}
};

template<class WidthTrait>
class ListLayoutVertical : public _ListLayoutVertical_Base, public SizeTrait<WidthTrait, Auto> {
public:
	using child_type = view_ptr<WidthTrait, Auto>;

protected:
	ListLayoutVertical(float gap, std::vector<ChildInfo> list) : _ListLayoutVertical_Base(gap, std::move(list)) {
		if constexpr (IsAuto<WidthTrait>) {
			for (auto& child : child_list) {
				child.region.size = UpdateChildSizeRef(child.view, Size(width_ref, length_zero));
			}
			size.width = CalculateMaxWidth();
			UpdateOffsetHeight(child_list.begin(), child_list.end());
		}
	}
public:
	ListLayoutVertical(float gap, std::vector<child_type> list) : ListLayoutVertical(gap, [&] {
		std::vector<ChildInfo> child_list; child_list.reserve(list.size());
		for (auto& child : list) { child_list.emplace_back(std::move(child)); }
		return child_list;
	}()) {}
	ListLayoutVertical(float gap, auto... child) requires (compatible_unique_ptr_type<decltype(child), child_type> && ...) : ListLayoutVertical(gap, [&] {
		std::vector<ChildInfo> child_list; child_list.reserve(sizeof...(child));
		(child_list.emplace_back(std::move(child)), ...);
		return child_list;
	}()) {}
	ListLayoutVertical(float gap, auto... child) requires (!compatible_unique_ptr_type<decltype(child), child_type> || ...) {
		static_assert((unique_ptr_type<decltype(child)> && ...), "ListLayoutVertical: child view argument of raw pointer type not accepted");
		static_assert((size_trait_compatible_with<decltype(child), child_type> && ...), "ListLayoutVertical: child view size traits incompatible");
	}

public:
	void Insert(size_t index, child_type child) {
		auto it = _ListLayout_Base::NormalizeInsertIndex(index);
		RegisterChild(child);
		it = _ListLayout_Base::Insert(it, std::move(child));
		it->region.size = UpdateChildSizeRef(it->view, Size(width_ref, length_zero));
		UpdateWidthHeight(CalculateMaxWidthAdd(it->region.size.width), UpdateOffset(it, it + 1));
	}
	void InsertBefore(ViewBase& ref, child_type child) { Insert(GetChildIndex(ref), std::move(child)); }
	void InsertAfter(ViewBase& ref, child_type child) { Insert(GetChildIndex(ref) + 1, std::move(child)); }
	void Append(child_type child) { Insert(-1, std::move(child)); }
	void Insert(size_t index, std::vector<child_type> list) {
		auto it = _ListLayout_Base::NormalizeInsertIndex(index);
		for (auto& child : list) { RegisterChild(child); }
		auto [begin, end] = _ListLayout_Base::Insert(it, std::move(list));
		float width = 0.0f;
		for (auto it = begin; it < end; ++it) {
			it->region.size = UpdateChildSizeRef(it->view, Size(width_ref, length_zero));
			if constexpr (!IsFixed<WidthTrait>) {
				width = std::max(width, it->region.size.width);
			}
		}
		UpdateWidthHeight(CalculateMaxWidthAdd(width), UpdateOffset(begin, end));
	}
	void InsertBefore(ViewBase& ref, std::vector<child_type> list) { Insert(GetChildIndex(ref), std::move(list)); }
	void InsertAfter(ViewBase& ref, std::vector<child_type> list) { Insert(GetChildIndex(ref) + 1, std::move(list)); }
	void Erase(size_t index) {
		auto it = _ListLayout_Base::NormalizeEraseIndex(index);
		auto [child, next] = _ListLayout_Base::Erase(it);
		UpdateWidthHeight(CalculateMaxWidthRemove(child.region.size.width), UpdateOffset(next));
		UnregisterChild(child.view);
	}
	void Erase(ViewBase& child) { Erase(GetChildIndex(child)); }
	void Erase(size_t index, size_t count) {
		auto [begin, end] = _ListLayout_Base::NormalizeEraseIndex(index, count);
		auto [list, next] = _ListLayout_Base::Erase(begin, end);
		UpdateWidthHeight(CalculateMaxWidth(), UpdateOffset(next));
		for (auto& child : list) { UnregisterChild(child.view); }
	}
	child_type Extract(size_t index) {
		auto it = _ListLayout_Base::NormalizeEraseIndex(index);
		auto [child, next] = _ListLayout_Base::Erase(it);
		UpdateWidthHeight(CalculateMaxWidthRemove(child.region.size.width), UpdateOffset(next));
		UnregisterChild(child.view);
		return std::move(child.view);
	}
	child_type Extract(ViewBase& child) { return Extract(GetChildIndex(child)); }
	std::vector<child_type> Extract(size_t index, size_t count) {
		auto [begin, end] = _ListLayout_Base::NormalizeEraseIndex(index, count);
		auto [list, next] = _ListLayout_Base::Erase(begin, end);
		UpdateWidthHeight(CalculateMaxWidth(), UpdateOffset(next));
		std::vector<child_type> result; result.reserve(list.size());
		for (auto& child : list) { UnregisterChild(child.view); result.push_back(std::move(child.view)); }
		return result;
	}

	// layout
protected:
	float CalculateMaxWidth() const {
		if constexpr (IsFixed<WidthTrait>) {
			return width_ref;
		} else {
			return child_list.empty() ? 0.0f : std::max_element(child_list.begin(), child_list.end(), [](const ChildInfo& a, const ChildInfo& b) { return a.region.size.width < b.region.size.width; })->region.size.width;
		}
	}
	float CalculateMaxWidthAdd(float child_width) const {
		if constexpr (IsFixed<WidthTrait>) {
			return width_ref;
		} else {
			return std::max(size.width, child_width);
		}
	}
	float CalculateMaxWidthRemove(float child_width) const {
		if constexpr (IsFixed<WidthTrait>) {
			return width_ref;
		} else {
			if (size.width == child_width) {
				return CalculateMaxWidth();
			} else {
				return size.width;
			}
		}
	}
protected:
	void UpdateWidthHeight(float width, float height) {
		if constexpr (IsFixed<WidthTrait>) {
			UpdateHeight(height);
		} else {
			if (size != Size(width, height)) {
				size = Size(width, height);
				SizeUpdated(size);
			}
		}
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if constexpr (!IsAuto<WidthTrait>) {
			if (width_ref != size_ref.width) {
				width_ref = size_ref.width;
				float y = 0.0f;
				for (auto& child : child_list) {
					child.region.point.y = y;
					child.region.size = UpdateChildSizeRef(child.view, Size(width_ref, length_zero));
					y += child.region.size.height + gap;
				}
				size = Size(CalculateMaxWidth(), child_list.empty() ? y : y - gap);
			}
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		auto it = child_list.begin() + GetChildIndex(child);
		if (IsFixed<WidthTrait> || it->region.size.width == child_size.width) {
			if (it->region.size.height != child_size.height) {
				it->region.size.height = child_size.height;
				UpdateOffsetHeight(it + 1);
			}
		} else {
			float child_width = it->region.size.width;
			it->region.size.width = child_size.width;
			float width = child_size.width >= child_width ? CalculateMaxWidthAdd(child_size.width) : CalculateMaxWidthRemove(child_width);
			if (size.width == width) {
				if (it->region.size.height != child_size.height) {
					it->region.size.height = child_size.height;
					UpdateOffsetHeight(it + 1);
				} else {
					RedrawChild(it);
				}
			} else {
				size.width = width;
				if (it->region.size.height != child_size.height) {
					it->region.size.height = child_size.height;
					size.height = UpdateOffset(it + 1);
				}
				SizeUpdated(size);
			}
		}
	}
};

template<class T>
ListLayoutVertical(float, std::vector<T>) -> ListLayoutVertical<extract_width_trait<T>>;

template<class T, class... Ts>
ListLayoutVertical(float, T, Ts...) -> ListLayoutVertical<extract_width_trait<T>>;

template<class ListLayoutVertical> requires std::derived_from<ListLayoutVertical, _ListLayoutVertical_Base>
class DeferredReflow<ListLayoutVertical> : public ListLayoutVertical {
protected:
	using Base = DeferredReflow;

public:
	using ListLayoutVertical::ListLayoutVertical;

protected:
	using ListLayoutVertical::child_list;
	using ListLayoutVertical::GetChildIndex;
	using ListLayoutVertical::size;
	using ListLayoutVertical::UpdateOffset;
	using ListLayoutVertical::CalculateMaxWidth;
	using ListLayoutVertical::SizeUpdated;
	using ListLayoutVertical::RedrawChild;

protected:
	size_t first = -1, last = 0;
	bool calculate_max_width = false;
	float width = 0.0f;
public:
	void Reflow() {
		if (calculate_max_width) {
			width = CalculateMaxWidth();
		} else {
			width = std::max(width, size.width);
		}
		if (size.width == width) {
			if (first <= last) {
				auto it_first = child_list.begin() + first, it_last = child_list.begin() + last;
				float height = UpdateOffset(it_first + 1, it_last + 1);
				if (size.height != height) {
					size.height = height;
					SizeUpdated(size);
				} else {
					RedrawChild(it_first, it_last);
				}
			}
		} else {
			size.width = width;
			if (first <= last) {
				auto it_first = child_list.begin() + first, it_last = child_list.begin() + last;
				size.height = UpdateOffset(it_first + 1, it_last + 1);
			}
			SizeUpdated(size);
		}
		first = -1; last = 0;
		calculate_max_width = false;
		width = 0.0f;
	}

protected:
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		auto index = GetChildIndex(child);
		auto it = child_list.begin() + index;
		if constexpr (IsFixed<typename ListLayoutVertical::width_trait>) {
			child_size.width = size.width;
		} else {
			width = std::max(width, child_size.width);
			if (!calculate_max_width) {
				if (width < it->region.size.width) {
					calculate_max_width = true;
				}
			} else {
				if (width >= size.width) {
					calculate_max_width = false;
				}
			}
		}
		if (it->region.size != child_size) {
			it->region.size = child_size;
			first = std::min(first, index);
			last = std::max(last, index);
		}
	}
};


class _ListLayoutHorizontal_Base : public _ListLayout_Base {
protected:
	using _ListLayout_Base::_ListLayout_Base;

	// layout
protected:
	float height_ref = 0.0f;
protected:
	std::vector<ChildInfo>::const_iterator HitTestIndex(float x) {
		return std::lower_bound(child_list.begin(), child_list.end(), x, [](const ChildInfo& child, float x) { return child.region.point.x <= x; }) - 1;
	}
protected:
	float UpdateOffset(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator end) {
		float x = it == child_list.begin() ? 0.0f : (it - 1)->region.right() + gap;
		for (; it != end; ++it) {
			it->region.point.x = x;
			x += it->region.size.width + gap;
		}
		for (; it != child_list.end(); ++it) {
			if (it->region.point.x == x) {
				return size.width;
			}
			it->region.point.x = x;
			x += it->region.size.width + gap;
		}
		return child_list.empty() ? x : x - gap;
	}
	float UpdateOffset(std::vector<ChildInfo>::iterator it) {
		return UpdateOffset(it, it);
	}
	void UpdateWidth(float width) {
		if (size.width != width) {
			size.width = width;
			SizeUpdated(size);
		}
	}
	void UpdateOffsetWidth(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator end) {
		UpdateWidth(UpdateOffset(it, end));
	}
	void UpdateOffsetWidth(std::vector<ChildInfo>::iterator it) {
		UpdateOffsetWidth(it, it);
	}

	// drawing
protected:
	void RedrawChild(std::vector<ChildInfo>::const_iterator it) {
		Redraw(Rect(Point(it->region.left(), 0), Size(it->region.width(), length_infinite)));
	}
	void RedrawChild(std::vector<ChildInfo>::const_iterator first, std::vector<ChildInfo>::const_iterator last) {
		Redraw(Rect(Point(first->region.left(), 0), Size(last->region.right() - first->region.left(), length_infinite)));
	}
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		auto begin = HitTestIndex(draw_region.left()), end = HitTestIndex(draw_region.right());
		for (auto it = begin; it <= end; ++it) {
			DrawChild(it->view, it->region, canvas, draw_region);
		}
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.point.x < 0.0f || event.point.x >= size.width) { return nullptr; }
		auto child = HitTestIndex(event.point.x);
		if (event.point.x >= child->region.right()) { return this; }
		event.point.x -= child->region.point.x;
		return child->view;
	}
};

template<class HeightTrait>
class ListLayoutHorizontal : public _ListLayoutHorizontal_Base, public SizeTrait<Auto, HeightTrait> {
public:
	using child_type = view_ptr<Auto, HeightTrait>;

protected:
	ListLayoutHorizontal(float gap, std::vector<ChildInfo> list) : _ListLayoutHorizontal_Base(gap, std::move(list)) {
		if constexpr (IsAuto<HeightTrait>) {
			for (auto& child : child_list) {
				child.region.size = UpdateChildSizeRef(child.view, Size(length_zero, height_ref));
			}
			size.height = CalculateMaxHeight();
			UpdateOffsetWidth(child_list.begin(), child_list.end());
		}
	}
public:
	ListLayoutHorizontal(float gap, std::vector<child_type> list) : ListLayoutHorizontal(gap, [&] {
		std::vector<ChildInfo> child_list; child_list.reserve(list.size());
		for (auto& child : list) { child_list.emplace_back(std::move(child)); }
		return child_list;
	}()) {}
	ListLayoutHorizontal(float gap, auto... child) requires (compatible_unique_ptr_type<decltype(child), child_type> && ...) : ListLayoutHorizontal(gap, [&] {
		std::vector<ChildInfo> child_list; child_list.reserve(sizeof...(child));
		(child_list.emplace_back(std::move(child)), ...);
		return child_list;
	}()) {}
	ListLayoutHorizontal(float gap, auto... child) requires (!compatible_unique_ptr_type<decltype(child), child_type> || ...) {
		static_assert((unique_ptr_type<decltype(child)> && ...), "ListLayoutHorizontal: child view argument of raw pointer type not accepted");
		static_assert((size_trait_compatible_with<decltype(child), child_type> && ...), "ListLayoutHorizontal: child view size traits incompatible");
	}

public:
	void Insert(size_t index, child_type child) {
		auto it = _ListLayout_Base::NormalizeInsertIndex(index);
		RegisterChild(child);
		it = _ListLayout_Base::Insert(it, std::move(child));
		it->region.size = UpdateChildSizeRef(it->view, Size(length_zero, height_ref));
		UpdateHeightWidth(CalculateMaxHeightAdd(it->region.size.height), UpdateOffset(it, it + 1));
	}
	void InsertBefore(ViewBase& ref, child_type child) { Insert(GetChildIndex(ref), std::move(child)); }
	void InsertAfter(ViewBase& ref, child_type child) { Insert(GetChildIndex(ref) + 1, std::move(child)); }
	void Append(child_type child) { Insert(-1, std::move(child)); }
	void Insert(size_t index, std::vector<child_type> list) {
		auto it = _ListLayout_Base::NormalizeInsertIndex(index);
		for (auto& child : list) { RegisterChild(child); }
		auto [begin, end] = _ListLayout_Base::Insert(it, std::move(list));
		float height = 0.0f;
		for (auto it = begin; it < end; ++it) {
			it->region.size = UpdateChildSizeRef(it->view, Size(length_zero, height_ref));
			if constexpr (!IsFixed<HeightTrait>) {
				height = std::max(height, it->region.size.height);
			}
		}
		UpdateHeightWidth(CalculateMaxHeightAdd(height), UpdateOffset(begin, end));
	}
	void InsertBefore(ViewBase& ref, std::vector<child_type> list) { Insert(GetChildIndex(ref), std::move(list)); }
	void InsertAfter(ViewBase& ref, std::vector<child_type> list) { Insert(GetChildIndex(ref) + 1, std::move(list)); }
	void Erase(size_t index) {
		auto it = _ListLayout_Base::NormalizeEraseIndex(index);
		auto [child, next] = _ListLayout_Base::Erase(it);
		UpdateHeightWidth(CalculateMaxHeightRemove(child.region.size.height), UpdateOffset(next));
		UnregisterChild(child.view);
	}
	void Erase(ViewBase& child) { Erase(GetChildIndex(child)); }
	void Erase(size_t index, size_t count) {
		auto [begin, end] = _ListLayout_Base::NormalizeEraseIndex(index, count);
		auto [list, next] = _ListLayout_Base::Erase(begin, end);
		UpdateHeightWidth(CalculateMaxHeight(), UpdateOffset(next));
		for (auto& child : list) { UnregisterChild(child.view); }
	}
	child_type Extract(size_t index) {
		auto it = _ListLayout_Base::NormalizeEraseIndex(index);
		auto [child, next] = _ListLayout_Base::Erase(it);
		UpdateHeightWidth(CalculateMaxHeightRemove(child.region.size.height), UpdateOffset(next));
		UnregisterChild(child.view);
		return std::move(child.view);
	}
	child_type Extract(ViewBase& child) { return Extract(GetChildIndex(child)); }
	std::vector<child_type> Extract(size_t index, size_t count) {
		auto [begin, end] = _ListLayout_Base::NormalizeEraseIndex(index, count);
		auto [list, next] = _ListLayout_Base::Erase(begin, end);
		UpdateHeightWidth(CalculateMaxHeight(), UpdateOffset(next));
		std::vector<child_type> result; result.reserve(list.size());
		for (auto& child : list) { UnregisterChild(child.view); result.push_back(std::move(child.view)); }
		return result;
	}

	// layout
protected:
	float CalculateMaxHeight() const {
		if constexpr (IsFixed<HeightTrait>) {
			return height_ref;
		} else {
			return child_list.empty() ? 0.0f : std::max_element(child_list.begin(), child_list.end(), [](const ChildInfo& a, const ChildInfo& b) { return a.region.size.height < b.region.size.height; })->region.size.height;
		}
	}
	float CalculateMaxHeightAdd(float child_height) const {
		if constexpr (IsFixed<HeightTrait>) {
			return height_ref;
		} else {
			return std::max(size.height, child_height);
		}
	}
	float CalculateMaxHeightRemove(float child_height) const {
		if constexpr (IsFixed<HeightTrait>) {
			return height_ref;
		} else {
			if (size.height == child_height) {
				return CalculateMaxHeight();
			} else {
				return size.height;
			}
		}
	}
protected:
	void UpdateHeightWidth(float height, float width) {
		if constexpr (IsFixed<HeightTrait>) {
			UpdateWidth(width);
		} else {
			if (size != Size(width, height)) {
				size = Size(width, height);
				SizeUpdated(size);
			}
		}
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if constexpr (!IsAuto<HeightTrait>) {
			if (height_ref != size_ref.height) {
				height_ref = size_ref.height;
				float x = 0.0f;
				for (auto& child : child_list) {
					child.region.point.x = x;
					child.region.size = UpdateChildSizeRef(child.view, Size(length_zero, height_ref));
					x += child.region.size.width + gap;
				}
				size = Size(child_list.empty() ? x : x - gap, CalculateMaxHeight());
			}
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		auto it = child_list.begin() + GetChildIndex(child);
		if (IsFixed<HeightTrait> || it->region.size.height == child_size.height) {
			if (it->region.size.width != child_size.width) {
				it->region.size.width = child_size.width;
				UpdateOffsetWidth(it + 1);
			}
		} else {
			float child_height = it->region.size.height;
			it->region.size.height = child_size.height;
			float height = child_size.height >= child_height ? CalculateMaxHeightAdd(child_size.height) : CalculateMaxHeightRemove(child_height);
			if (size.height == height) {
				if (it->region.size.width != child_size.width) {
					it->region.size.width = child_size.width;
					UpdateOffsetWidth(it + 1);
				} else {
					RedrawChild(it);
				}
			} else {
				size.height = height;
				if (it->region.size.width != child_size.width) {
					it->region.size.width = child_size.width;
					size.width = UpdateOffset(it + 1);
				}
				SizeUpdated(size);
			}
		}
	}
};

template<class T>
ListLayoutHorizontal(float, std::vector<T>) -> ListLayoutHorizontal<extract_height_trait<T>>;

template<class T, class... Ts>
ListLayoutHorizontal(float, T, Ts...) -> ListLayoutHorizontal<extract_height_trait<T>>;

template<class ListLayoutHorizontal> requires std::derived_from<ListLayoutHorizontal, _ListLayoutHorizontal_Base>
class DeferredReflow<ListLayoutHorizontal> : public ListLayoutHorizontal {
protected:
	using Base = DeferredReflow;

public:
	using ListLayoutHorizontal::ListLayoutHorizontal;

protected:
	using ListLayoutHorizontal::child_list;
	using ListLayoutHorizontal::GetChildIndex;
	using ListLayoutHorizontal::size;
	using ListLayoutHorizontal::UpdateOffset;
	using ListLayoutHorizontal::CalculateMaxHeight;
	using ListLayoutHorizontal::SizeUpdated;
	using ListLayoutHorizontal::RedrawChild;

protected:
	size_t first = -1, last = 0;
	bool calculate_max_height = false;
	float height = 0.0f;
public:
	void Reflow() {
		if (calculate_max_height) {
			height = CalculateMaxHeight();
		} else {
			height = std::max(height, size.height);
		}
		if (size.height == height) {
			if (first <= last) {
				auto it_first = child_list.begin() + first, it_last = child_list.begin() + last;
				float width = UpdateOffset(it_first + 1, it_last + 1);
				if (size.width != width) {
					size.width = width;
					SizeUpdated(size);
				} else {
					RedrawChild(it_first, it_last);
				}
			}
		} else {
			size.height = height;
			if (first <= last) {
				auto it_first = child_list.begin() + first, it_last = child_list.begin() + last;
				size.width = UpdateOffset(it_first + 1, it_last + 1);
			}
			SizeUpdated(size);
		}
		first = -1; last = 0;
		calculate_max_height = false;
		height = 0.0f;
	}

protected:
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		auto index = GetChildIndex(child);
		auto it = child_list.begin() + index;
		if constexpr (IsFixed<typename ListLayoutHorizontal::height_trait>) {
			child_size.height = size.height;
		} else {
			height = std::max(height, child_size.height);
			if (!calculate_max_height) {
				if (height < it->region.size.height) {
					calculate_max_height = true;
				}
			} else {
				if (height >= size.height) {
					calculate_max_height = false;
				}
			}
		}
		if (it->region.size != child_size) {
			it->region.size = child_size;
			first = std::min(first, index);
			last = std::max(last, index);
		}
	}
};


} // namespace ViewDesign

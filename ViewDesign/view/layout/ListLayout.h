#pragma once

#include "ViewDesign/view/view_traits.h"

#include <vector>


namespace ViewDesign {

template<class View>
class DelayedReflow;


class _ListLayout_Base : public ViewBase {
protected:
	struct ChildInfo {
	public:
		view_ptr<> view;
		Rect region;
	public:
		ChildInfo(view_ptr<> view) : view(std::move(view)), region(region_empty) {}
	};

private:
	_ListLayout_Base(float gap, std::vector<ChildInfo> child_list) : gap(gap), child_list(std::move(child_list)) {
		size_t index = 0;
		for (auto& child : this->child_list) {
			RegisterChild(child.view);
			SetChildIndex(child.view, index++);
		}
	}
protected:
	_ListLayout_Base(float gap, auto... child) : _ListLayout_Base(gap, [&]() {
		std::vector<ChildInfo> child_list; child_list.reserve(sizeof...(child));
		(child_list.emplace_back(std::move(child)), ...);
		return child_list;
	}()) {}

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
protected:
	size_t GetChildIndex(ViewBase& child) const { return ViewBase::GetChildData<size_t>(child); }
	void SetChildIndex(ViewBase& child, size_t index) { ViewBase::SetChildData<size_t>(child, index); }
protected:
	void UpdateIndex(std::vector<ChildInfo>::iterator it) {
		for (; it != child_list.end(); ++it) {
			SetChildIndex(it->view, it - child_list.begin());
		}
	}
protected:
	std::vector<ChildInfo>::iterator InsertChild(size_t index, view_ptr<> child) {
		if (index > child_list.size()) {
			index = child_list.size();
		}
		RegisterChild(child);
		auto it = child_list.emplace(child_list.begin() + index, std::move(child));
		UpdateIndex(it);
		return it;
	}
	std::pair<std::vector<ChildInfo>::iterator, std::vector<ChildInfo>::iterator> InsertChild(size_t index, auto list) requires std::ranges::input_range<decltype(list)> {
		if (index > child_list.size()) {
			index = child_list.size();
		}
		for (auto& child : list) {
			RegisterChild(child);
		}
		auto it = child_list.insert(child_list.begin() + index, std::make_move_iterator(list.begin()), std::make_move_iterator(list.end()));
		UpdateIndex(it);
		return std::make_pair(it, it + list.size());
	}
	std::pair<ChildInfo, std::vector<ChildInfo>::iterator> EraseChild(size_t index) {
		if (index >= child_list.size()) {
			throw std::invalid_argument("invalid index");
		}
		ChildInfo child = std::move(child_list[index]);
		auto it = child_list.erase(child_list.begin() + index);
		UpdateIndex(it);
		return std::make_pair(std::move(child), it);
	}
	std::pair<std::vector<ChildInfo>, std::vector<ChildInfo>::iterator> EraseChild(size_t index, size_t count) {
		if (index >= child_list.size()) {
			throw std::invalid_argument("invalid index");
		}
		if (count == 0) {
			return { std::vector<ChildInfo>(), child_list.end() };
		}
		count = std::min(count, child_list.size() - index);
		size_t end = index + count;
		std::vector<ChildInfo> list(std::make_move_iterator(child_list.begin() + index), std::make_move_iterator(child_list.begin() + end));
		auto it = child_list.erase(child_list.begin() + index, child_list.begin() + end);
		UpdateIndex(it);
		return std::make_pair(std::move(list), it);
	}

	// layout
protected:
	Size size;
protected:
	Rect GetChildRegion(ViewBase& child) const { return child_list[GetChildIndex(child)].region; }
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
	float UpdateOffset(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator it_end) {
		float y = it == child_list.begin() ? 0.0f : (it - 1)->region.bottom() + gap;
		for (; it != it_end; ++it) {
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
	void UpdateOffsetHeight(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator it_end) {
		UpdateHeight(UpdateOffset(it, it_end));
	}
	void UpdateOffsetHeight(std::vector<ChildInfo>::iterator it) {
		UpdateOffsetHeight(it, it);
	}

	// drawing
protected:
	void RedrawChild(std::vector<ChildInfo>::const_iterator it) {
		Redraw(Rect(Point(0, it->region.top()), Size(length_max, it->region.height())));
	}
	void RedrawChild(std::vector<ChildInfo>::const_iterator first, std::vector<ChildInfo>::const_iterator last) {
		Redraw(Rect(Point(0, first->region.top()), Size(length_max, last->region.bottom() - first->region.top())));
	}
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		auto it_begin = HitTestIndex(draw_region.top()), it_end = HitTestIndex(draw_region.bottom());
		for (auto it = it_begin; it <= it_end; ++it) {
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

public:
	ListLayoutVertical(auto&&... args) : _ListLayoutVertical_Base(std::forward<decltype(args)>(args)...) {
		if constexpr (IsAuto<WidthTrait>) {
			for (auto& child : child_list) {
				child.region.size = UpdateChildSizeRef(child.view, Size(width_ref, length_min));
			}
			size.width = CalculateMaxWidth();
			UpdateOffsetHeight(child_list.begin(), child_list.end());
		}
	}

public:
	void InsertChild(size_t index, child_type child) {
		auto it = _ListLayout_Base::InsertChild(index, std::move(child));
		it->region.size = UpdateChildSizeRef(it->view, Size(width_ref, length_min));
		UpdateWidthHeight(CalculateMaxWidthAdd(it->region.size.width), UpdateOffset(it, it + 1));
	}
	void AppendChild(child_type child) { InsertChild(-1, std::move(child)); }
	void InsertChild(size_t begin, std::vector<child_type> list) {
		auto [it_begin, it_end] = _ListLayout_Base::InsertChild(begin, std::move(list));
		float width = 0.0f;
		for (auto it = it_begin; it < it_end; ++it) {
			it->region.size = UpdateChildSizeRef(it->view, Size(width_ref, length_min));
			if constexpr (!IsFixed<WidthTrait>) {
				width = std::max(width, it->region.size.width);
			}
		}
		UpdateWidthHeight(CalculateMaxWidthAdd(width), UpdateOffset(it_begin, it_end));
	}
	void EraseChild(size_t index) {
		auto [child, it] = _ListLayout_Base::EraseChild(index);
		UpdateWidthHeight(CalculateMaxWidthRemove(child.region.size.width), UpdateOffset(it));
	}
	void EraseChild(size_t index, size_t count) {
		auto [list, it] = _ListLayout_Base::EraseChild(index, count);
		UpdateWidthHeight(CalculateMaxWidth(), UpdateOffset(it));
	}
	child_type ExtractChild(size_t index) {
		auto [child, it] = _ListLayout_Base::EraseChild(index);
		UpdateWidthHeight(CalculateMaxWidthRemove(child.region.size.width), UpdateOffset(it));
		UnregisterChild(child.view);
		return std::move(child.view);
	}
	std::vector<child_type> ExtractChild(size_t index, size_t count) {
		auto [list, it] = _ListLayout_Base::EraseChild(index, count);
		UpdateWidthHeight(CalculateMaxWidth(), UpdateOffset(it));
		std::vector<child_type> result; result.reserve(list.size());
		for (auto& child : list) {
			UnregisterChild(child.view);
			result.push_back(std::move(child.view));
		}
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
					child.region.size = UpdateChildSizeRef(child.view, Size(width_ref, length_min));
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

template<class T, class... Ts>
ListLayoutVertical(float, T, Ts...) -> ListLayoutVertical<extract_width_trait<T>>;


template<class ListLayoutVertical> requires std::derived_from<_ListLayoutVertical_Base, ListLayoutVertical>
class DelayedReflow<ListLayoutVertical> : public ListLayoutVertical {
protected:
	using Base = DelayedReflow;

public:
	using ListLayoutVertical::ListLayoutVertical;

protected:
	using ListLayoutVertical::child_list;
	using ListLayoutVertical::GetChildIndex;
	using ListLayoutVertical::size;
	using ListLayoutVertical::UpdateOffset;
	using ListLayoutVertical::CalculateMaxWidth;

protected:
	size_t first = 0, last = 0;
	bool calculate_max_width = false;
	float width = 0.0f;
public:
	void Reflow() {
		if (calculate_max_width) {
			width = CalculateMaxWidth();
			calculate_max_width = false;
		} else {
			width = std::max(width, size.width);
		}
		auto it_first = child_list.begin() + first, it_last = child_list.begin() + last;
		if (size.width == width) {
			float height = UpdateOffset(it_first + 1, it_last + 1);
			if (size.height != height) {
				size.height = height;
				SizeUpdated(size);
			} else {
				RedrawChild(it_first, it_last);
			}
		} else {
			size.width = width;
			size.height = UpdateOffset(it_first + 1, it_last + 1);
			SizeUpdated(size);
		}
	}

protected:
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		auto index = GetChildIndex(child);
		auto it = child_list.begin() + index;
		if (it->region.size.height != child_size.height) {
			it->region.size.height = child_size.height;
			first = std::min(first, index);
			last = std::max(last, index);
		}
		if constexpr (!IsFixed<typename ListLayoutVertical::width_trait>) {
			if (child_size.width > it->region.size.width) {
				it->region.size.width = child_size.width;
				width = std::max(width, child_size.width);
			} else if (child_size.width < it->region.size.width) {
				it->region.size.width = child_size.width;
				calculate_max_width = true;
			}
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
	float UpdateOffset(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator it_end) {
		float x = it == child_list.begin() ? 0.0f : (it - 1)->region.right() + gap;
		for (; it != it_end; ++it) {
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
	void UpdateOffsetWidth(std::vector<ChildInfo>::iterator it, std::vector<ChildInfo>::iterator it_end) {
		UpdateWidth(UpdateOffset(it, it_end));
	}
	void UpdateOffsetWidth(std::vector<ChildInfo>::iterator it) {
		UpdateOffsetWidth(it, it);
	}

	// drawing
protected:
	void RedrawChild(std::vector<ChildInfo>::const_iterator it) {
		Redraw(Rect(Point(it->region.left(), 0), Size(it->region.width(), length_max)));
	}
	void RedrawChild(std::vector<ChildInfo>::const_iterator first, std::vector<ChildInfo>::const_iterator last) {
		Redraw(Rect(Point(first->region.left(), 0), Size(last->region.right() - first->region.left(), length_max)));
	}
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		auto it_begin = HitTestIndex(draw_region.left()), it_end = HitTestIndex(draw_region.right());
		for (auto it = it_begin; it <= it_end; ++it) {
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

public:
	ListLayoutHorizontal(auto&&... args) : _ListLayoutHorizontal_Base(std::forward<decltype(args)>(args)...) {
		if constexpr (IsAuto<HeightTrait>) {
			for (auto& child : child_list) {
				child.region.size = UpdateChildSizeRef(child.view, Size(length_min, height_ref));
			}
			size.height = CalculateMaxHeight();
			UpdateOffsetWidth(child_list.begin(), child_list.end());
		}
	}

public:
	void InsertChild(size_t index, child_type child) {
		auto it = _ListLayout_Base::InsertChild(index, std::move(child));
		it->region.size = UpdateChildSizeRef(it->view, Size(length_min, height_ref));
		UpdateHeightWidth(CalculateMaxHeightAdd(it->region.size.height), UpdateOffset(it, it + 1));
	}
	void AppendChild(child_type child) { InsertChild(-1, std::move(child)); }
	void InsertChild(size_t begin, std::vector<child_type> list) {
		auto [it_begin, it_end] = _ListLayout_Base::InsertChild(begin, std::move(list));
		float height = 0.0f;
		for (auto it = it_begin; it < it_end; ++it) {
			it->region.size = UpdateChildSizeRef(it->view, Size(length_min, height_ref));
			if constexpr (!IsFixed<HeightTrait>) {
				height = std::max(height, it->region.size.height);
			}
		}
		UpdateHeightWidth(CalculateMaxHeightAdd(height), UpdateOffset(it_begin, it_end));
	}
	void EraseChild(size_t index) {
		auto [child, it] = _ListLayout_Base::EraseChild(index);
		UpdateHeightWidth(CalculateMaxHeightRemove(child.region.size.height), UpdateOffset(it));
	}
	void EraseChild(size_t index, size_t count) {
		auto [list, it] = _ListLayout_Base::EraseChild(index, count);
		UpdateHeightWidth(CalculateMaxHeight(), UpdateOffset(it));
	}
	child_type ExtractChild(size_t index) {
		auto [child, it] = _ListLayout_Base::EraseChild(index);
		UpdateHeightWidth(CalculateMaxHeightRemove(child.region.size.height), UpdateOffset(it));
		UnregisterChild(child.view);
		return std::move(child.view);
	}
	std::vector<child_type> ExtractChild(size_t index, size_t count) {
		auto [list, it] = _ListLayout_Base::EraseChild(index, count);
		UpdateHeightWidth(CalculateMaxHeight(), UpdateOffset(it));
		std::vector<child_type> result; result.reserve(list.size());
		for (auto& child : list) {
			UnregisterChild(child.view);
			result.push_back(std::move(child.view));
		}
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
					child.region.size = UpdateChildSizeRef(child.view, Size(length_min, height_ref));
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

template<class T, class... Ts>
ListLayoutHorizontal(float, T, Ts...) -> ListLayoutHorizontal<extract_height_trait<T>>;


template<class ListLayoutHorizontal> requires std::derived_from<_ListLayoutHorizontal_Base, ListLayoutHorizontal>
class DelayedReflow<ListLayoutHorizontal> : public ListLayoutHorizontal {
protected:
	using Base = DelayedReflow;

public:
	using ListLayoutHorizontal::ListLayoutHorizontal;

protected:
	using ListLayoutHorizontal::child_list;
	using ListLayoutHorizontal::GetChildIndex;
	using ListLayoutHorizontal::size;
	using ListLayoutHorizontal::UpdateOffset;
	using ListLayoutHorizontal::CalculateMaxHeight;

protected:
	size_t first = 0, last = 0;
	bool calculate_max_height = false;
	float height = 0.0f;
public:
	void Reflow() {
		if (calculate_max_height) {
			height = CalculateMaxHeight();
			calculate_max_height = false;
		} else {
			height = std::max(height, size.height);
		}
		auto it_first = child_list.begin() + first, it_last = child_list.begin() + last;
		if (size.height == height) {
			float width = UpdateOffset(it_first + 1, it_last + 1);
			if (size.width != width) {
				size.width = width;
				SizeUpdated(size);
			} else {
				RedrawChild(it_first, it_last);
			}
		} else {
			size.height = height;
			size.width = UpdateOffset(it_first + 1, it_last + 1);
			SizeUpdated(size);
		}
	}

protected:
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		auto index = GetChildIndex(child);
		auto it = child_list.begin() + index;
		if (it->region.size.width != child_size.width) {
			it->region.size.width = child_size.width;
			first = std::min(first, index);
			last = std::max(last, index);
		}
		if constexpr (!IsFixed<typename ListLayoutHorizontal::height_trait>) {
			if (child_size.height > it->region.size.height) {
				it->region.size.height = child_size.height;
				height = std::max(height, child_size.height);
			} else if (child_size.height < it->region.size.height) {
				it->region.size.height = child_size.height;
				calculate_max_height = true;
			}
		}
	}
};


} // namespace ViewDesign

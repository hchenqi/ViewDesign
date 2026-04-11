#pragma once

#include "ListLayout.h"

#include <algorithm>


namespace ViewDesign {


void ListLayout<Vertical>::UpdateIndex(size_t begin) {
	for (size_t index = begin; index < child_list.size(); ++index) {
		SetChildIndex(child_list[index].child, index);
	}
}

void ListLayout<Vertical>::InsertChild(size_t index, child_type child) {
	RegisterChild(child);
	if (index > child_list.size()) { index = child_list.size(); }
	auto it = child_list.emplace(child_list.begin() + index, std::move(child));
	UpdateIndex(index);
	it->length = UpdateChildSizeRef(it->child, Size(size.width, length_min)).height;
	UpdateLayout(index);
}

void ListLayout<Vertical>::InsertChild(size_t begin, std::vector<child_type> children) {
	for (auto& child : children) { RegisterChild(child); }
	if (begin > child_list.size()) { begin = child_list.size(); }
	auto it = child_list.insert(child_list.begin() + begin, std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
	UpdateIndex(begin);
	for (auto it_end = it + children.size(); it != it_end; ++it) {
		it->length = UpdateChildSizeRef(it->child, Size(size.width, length_min)).height;
	}
	UpdateLayout(begin);
}

void ListLayout<Vertical>::EraseChild(size_t begin, size_t count) {
	if (begin >= child_list.size() || count == 0) { return; }
	count = std::min(count, child_list.size() - begin); size_t end = begin + count;
	child_list.erase(child_list.begin() + begin, child_list.begin() + end);
	UpdateIndex(begin); UpdateLayout(begin);
}

ListLayout<Vertical>::child_type ListLayout<Vertical>::ExtractChild(size_t index) {
	if (index >= child_list.size()) { throw std::invalid_argument("invalid child index"); }
	UnregisterChild(child_list[index].child);
	view_ptr ptr(std::move(child_list[index].child));
	EraseChild(index, 1);
	return ptr;
}

std::vector<ListLayout<Vertical>::child_type> ListLayout<Vertical>::ExtractChild(size_t begin, size_t count) {
	if (begin >= child_list.size() || count == 0) { return {}; }
	count = std::min(count, child_list.size() - begin); size_t end = begin + count;
	std::vector<child_type> ptr_list; ptr_list.reserve(count);
	for (size_t index = begin; index < end; ++index) {
		UnregisterChild(child_list[index].child);
		ptr_list.emplace_back(std::move(child_list[index].child));
	}
	EraseChild(begin, count);
	return ptr_list;
}

void ListLayout<Vertical>::UpdateLayout(size_t index) {
	size.height = index == 0 ? (Empty() ? 0.0f : -gap) : GetChildRegion(index - 1).bottom();
	for (index; index < child_list.size(); index++) {
		size.height += gap;
		child_list[index].offset = size.height;
		size.height += child_list[index].length;
	}
	SizeUpdated(size);
}

size_t ListLayout<Vertical>::HitTestIndex(Point point) {
	static auto cmp = [](const ChildInfo& item, float offset) { return offset >= item.offset; };
	return std::lower_bound(child_list.begin(), child_list.end(), point.y, cmp) - child_list.begin() - 1;
}

Transform ListLayout<Vertical>::GetChildTransform(ViewBase& child) const {
	return GetChildRegion(child).point - point_zero;
}

Size ListLayout<Vertical>::OnSizeRefUpdate(Size size_ref) {
	if (size.width != size_ref.width) {
		size.width = size_ref.width;
		size.height = 0.0f;
		for (auto& info : child_list) {
			info.offset = size.height;
			info.length = UpdateChildSizeRef(*info.child, Size(size.width, length_min)).height;
			size.height += info.length + gap;
		}
		size.height -= child_list.empty() ? 0.0f : gap;
	}
	return size;
}

void ListLayout<Vertical>::OnChildSizeUpdate(ViewBase& child, Size child_size) {
	size_t index = GetChildIndex(child);
	if (child_list[index].length != child_size.height) {
		child_list[index].length = child_size.height;
		UpdateLayout(index + 1);
	}
}

void ListLayout<Vertical>::OnChildRedraw(ViewBase& child, Rect child_redraw_region) {
	Rect child_region = GetChildRegion(child);
	Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
}

void ListLayout<Vertical>::OnDraw(FigureQueue& figure_queue, Rect draw_region) {
	draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
	size_t index_begin = HitTestIndex(draw_region.LeftTop()), index_end = HitTestIndex(draw_region.RightBottom());
	for (size_t index = index_begin; index <= index_end; ++index) { DrawChild(GetChild(index), GetChildRegion(index), figure_queue, draw_region); }
}

ref_ptr<ViewBase> ListLayout<Vertical>::HitTest(MouseEvent& event) {
	size_t index = HitTestIndex(event.point); if (index >= Length()) { return this; }
	Rect child_region = GetChildRegion(index); if (!child_region.Contains(event.point)) { return this; }
	event.point -= child_region.point - point_zero;
	return &GetChild(index);
}


void ListLayout<Horizontal>::UpdateIndex(size_t begin) {
	for (size_t index = begin; index < child_list.size(); ++index) {
		SetChildIndex(child_list[index].child, index);
	}
}

void ListLayout<Horizontal>::InsertChild(size_t index, child_type child) {
	RegisterChild(child);
	if (index > child_list.size()) { index = child_list.size(); }
	auto it = child_list.emplace(child_list.begin() + index, std::move(child));
	UpdateIndex(index);
	it->length = UpdateChildSizeRef(it->child, Size(length_min, size.height)).width;
	UpdateLayout(index);
}

void ListLayout<Horizontal>::InsertChild(size_t begin, std::vector<child_type> children) {
	for (auto& child : children) { RegisterChild(child); }
	if (begin > child_list.size()) { begin = child_list.size(); }
	auto it = child_list.insert(child_list.begin() + begin, std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
	UpdateIndex(begin);
	for (auto it_end = it + children.size(); it != it_end; ++it) {
		it->length = UpdateChildSizeRef(it->child, Size(length_min, size.height)).width;
	}
	UpdateLayout(begin);
}

void ListLayout<Horizontal>::EraseChild(size_t begin, size_t count) {
	if (begin >= child_list.size() || count == 0) { return; }
	count = std::min(count, child_list.size() - begin); size_t end = begin + count;
	child_list.erase(child_list.begin() + begin, child_list.begin() + end);
	UpdateIndex(begin); UpdateLayout(begin);
}

ListLayout<Horizontal>::child_type ListLayout<Horizontal>::ExtractChild(size_t index) {
	UnregisterChild(child_list[index].child);
	child_type item(std::move(child_list[index].child));
	EraseChild(index, 1);
	return item;
}

std::vector<ListLayout<Horizontal>::child_type> ListLayout<Horizontal>::ExtractChild(size_t begin, size_t count) {
	if (begin >= child_list.size() || count == 0) { return {}; }
	count = std::min(count, child_list.size() - begin); size_t end = begin + count;
	std::vector<child_type> children; children.reserve(count);
	for (size_t index = begin; index < end; ++index) {
		UnregisterChild(child_list[index].child);
		children.emplace_back(std::move(child_list[index].child));
	}
	EraseChild(begin, count);
	return children;
}

void ListLayout<Horizontal>::UpdateLayout(size_t index) {
	size.width = index == 0 ? (child_list.size() == 0 ? 0.0f : -gap) : GetChildRegion(index - 1).right();
	for (index; index < child_list.size(); index++) {
		size.width += gap;
		child_list[index].offset = size.width;
		size.width += child_list[index].length;
	}
	SizeUpdated(size);
}

size_t ListLayout<Horizontal>::HitTestIndex(Point point) {
	static auto cmp = [](const ChildInfo& item, float offset) { return offset >= item.offset; };
	return std::lower_bound(child_list.begin(), child_list.end(), point.x, cmp) - child_list.begin() - 1;
}

Transform ListLayout<Horizontal>::GetChildTransform(ViewBase& child) const {
	return GetChildRegion(child).point - point_zero;
}

Size ListLayout<Horizontal>::OnSizeRefUpdate(Size size_ref) {
	if (size.height != size_ref.height) {
		size.height = size_ref.height;
		size.width = 0.0f;
		for (auto& info : child_list) {
			info.offset = size.width;
			info.length = UpdateChildSizeRef(*info.child, Size(length_min, size.height)).width;
			size.width += info.length + gap;
		}
		size.width -= child_list.empty() ? 0.0f : gap;
	}
	return size;
}

void ListLayout<Horizontal>::OnChildSizeUpdate(ViewBase& child, Size child_size) {
	size_t index = GetChildIndex(child);
	if (child_list[index].length != child_size.width) {
		child_list[index].length = child_size.width;
		UpdateLayout(index + 1);
	}
}

void ListLayout<Horizontal>::OnChildRedraw(ViewBase& child, Rect child_redraw_region) {
	Rect child_region = GetChildRegion(child);
	Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
}

void ListLayout<Horizontal>::OnDraw(FigureQueue& figure_queue, Rect draw_region) {
	draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
	size_t index_begin = HitTestIndex(draw_region.LeftTop()), index_end = HitTestIndex(draw_region.RightBottom());
	for (size_t index = index_begin; index <= index_end; ++index) { DrawChild(GetChild(index), GetChildRegion(index), figure_queue, draw_region); }
}

ref_ptr<ViewBase> ListLayout<Horizontal>::HitTest(MouseEvent& event) {
	size_t index = HitTestIndex(event.point); if (index >= Length()) { return this; }
	Rect child_region = GetChildRegion(index); if (!child_region.Contains(event.point)) { return this; }
	event.point -= child_region.point - point_zero;
	return &GetChild(index);
}


} // namespace ViewDesign

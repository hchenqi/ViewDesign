#pragma once

#include "ListLayoutAuto.h"

#include <algorithm>


namespace ViewDesign {


void _ListLayoutAuto_Base::UpdateIndex(size_t begin) {
	for (size_t index = begin; index < child_list.size(); ++index) {
		SetChildIndex(child_list[index].child, index);
	}
}

size_t _ListLayoutAuto_Base::InsertChild(size_t index, child_type child) {
	RegisterChild(child);
	if (index > child_list.size()) { index = child_list.size(); }
	auto it = child_list.emplace(child_list.begin() + index, std::move(child));
	UpdateIndex(index);
	it->region.size = UpdateChildSizeRef(it->child, size_empty);
	return index;
}

size_t _ListLayoutAuto_Base::InsertChild(size_t begin, std::vector<child_type> children) {
	for (auto& child : children) { RegisterChild(child); }
	if (begin > child_list.size()) { begin = child_list.size(); }
	auto it = child_list.insert(child_list.begin() + begin, std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
	UpdateIndex(begin);
	for (auto it_end = it + children.size(); it < it_end; ++it) {
		it->region.size = UpdateChildSizeRef(it->child, size_empty);
	}
	return begin;
}

void _ListLayoutAuto_Base::EraseChild(size_t begin, size_t count) {
	if (begin >= child_list.size() || count == 0) { return; }
	count = std::min(count, child_list.size() - begin); size_t end = begin + count;
	child_list.erase(child_list.begin() + begin, child_list.begin() + end);
	UpdateIndex(begin);
}

_ListLayoutAuto_Base::child_type _ListLayoutAuto_Base::ExtractChild(size_t index) {
	UnregisterChild(child_list[index].child);
	return std::move(child_list[index].child);
}

std::vector<_ListLayoutAuto_Base::child_type> _ListLayoutAuto_Base::ExtractChild(size_t begin, size_t count) {
	if (begin >= child_list.size() || count == 0) { return {}; }
	count = std::min(count, child_list.size() - begin); size_t end = begin + count;
	std::vector<child_type> ptr_list; ptr_list.reserve(count);
	for (size_t index = begin; index < end; ++index) {
		UnregisterChild(child_list[index].child);
		ptr_list.emplace_back(std::move(child_list[index].child));
	}
	return ptr_list;
}

Transform _ListLayoutAuto_Base::GetChildTransform(WndObject& child) const {
	return GetChildRegion(child).point - point_zero;
}

Size _ListLayoutAuto_Base::OnSizeRefUpdate(Size size_ref) {
	return size;
}

void _ListLayoutAuto_Base::OnChildRedraw(WndObject& child, Rect child_redraw_region) {
	Rect child_region = GetChildRegion(child);
	Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
}


void ListLayoutAuto<Vertical>::InsertChild(size_t index, child_type child) {
	index = _ListLayoutAuto_Base::InsertChild(index, std::move(child));
	size.width = std::max(size.width, child_list[index].region.size.width);
	UpdateLayout(index);
}

void ListLayoutAuto<Vertical>::InsertChild(size_t begin, std::vector<child_type> children) {
	size_t length = children.size();
	begin = _ListLayoutAuto_Base::InsertChild(begin, std::move(children));
	for (size_t index = begin, end = begin + length; index < end; ++index) {
		size.width = std::max(size.width, child_list[index].region.size.width);
	}
	UpdateLayout(begin);
}

void ListLayoutAuto<Vertical>::EraseChild(size_t begin, size_t count) {
	_ListLayoutAuto_Base::EraseChild(begin, count);
	size.width = 0.0f; for (auto& child : child_list) { size.width = std::max(size.width, child.region.size.width); }
	UpdateLayout(begin);
}

ListLayoutAuto<Vertical>::child_type ListLayoutAuto<Vertical>::ExtractChild(size_t index) {
	child_ptr ptr = _ListLayoutAuto_Base::ExtractChild(index);
	EraseChild(index, 1);
	return ptr;
}

std::vector<ListLayoutAuto<Vertical>::child_type> ListLayoutAuto<Vertical>::ExtractChild(size_t begin, size_t count) {
	std::vector<child_type> ptr_list = _ListLayoutAuto_Base::ExtractChild(begin, count);
	EraseChild(begin, count);
	return ptr_list;
}

void ListLayoutAuto<Vertical>::UpdateLayout(size_t index) {
	size.height = index == 0 ? (Empty() ? 0.0f : -gap) : child_list[index - 1].region.bottom();
	for (index; index < child_list.size(); index++) {
		size.height += gap;
		child_list[index].region.point.y = size.height;
		size.height += child_list[index].region.size.height;
	}
	SizeUpdated(size);
}

size_t ListLayoutAuto<Vertical>::HitTestIndex(Point point) {
	static auto cmp = [](const ChildInfo& item, float offset) { return offset >= item.region.point.y; };
	return std::lower_bound(child_list.begin(), child_list.end(), point.y, cmp) - child_list.begin() - 1;
}

void ListLayoutAuto<Vertical>::OnChildSizeUpdate(WndObject& child, Size child_size) {
	size_t index = GetChildIndex(child); ChildInfo& info = child_list[index];
	Size child_size_old = info.region.size; float width_old = size.width;
	info.region.size = child_size;
	if (size.width == child_size_old.width && child_size.width < size.width) {
		size.width = 0.0f; for (auto& child : child_list) { size.width = std::max(size.width, child.region.size.width); }
	} else if (size.width < child_size.width) {
		size.width = child_size.width;
	}
	if (child_size.height != child_size_old.height) {
		UpdateLayout(index + 1);
	} else if (size.width != width_old) {
		SizeUpdated(size);
	} else if (child_size.width != child_size_old.width) {
		Redraw(Rect(info.region.point, Size(std::max(child_size.width, child_size_old.width), child_size.height)));
	}
}

void ListLayoutAuto<Vertical>::OnDraw(FigureQueue& figure_queue, Rect draw_region) {
	draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
	size_t index_begin = HitTestIndex(draw_region.LeftTop()), index_end = HitTestIndex(draw_region.RightBottom());
	for (size_t index = index_begin; index <= index_end; ++index) { DrawChild(GetChild(index), GetChildRegion(index), figure_queue, draw_region); }
}

ref_ptr<WndObject> ListLayoutAuto<Vertical>::HitTest(MouseMsg& msg) {
	size_t index = HitTestIndex(msg.point); if (index >= Length()) { return this; }
	Rect child_region = GetChildRegion(index); if (!child_region.Contains(msg.point)) { return this; }
	msg.point -= child_region.point - point_zero;
	return &GetChild(index);
}


void ListLayoutAuto<Horizontal>::InsertChild(size_t index, child_type child) {
	index = _ListLayoutAuto_Base::InsertChild(index, std::move(child));
	size.height = std::max(size.height, child_list[index].region.size.height);
	UpdateLayout(index);
}

void ListLayoutAuto<Horizontal>::InsertChild(size_t begin, std::vector<child_type> children) {
	size_t length = children.size();
	begin = _ListLayoutAuto_Base::InsertChild(begin, std::move(children));
	for (size_t index = begin, end = begin + length; index < end; ++index) {
		size.height = std::max(size.height, child_list[index].region.size.height);
	}
	UpdateLayout(begin);
}

void ListLayoutAuto<Horizontal>::EraseChild(size_t begin, size_t count) {
	_ListLayoutAuto_Base::EraseChild(begin, count);
	size.height = 0.0f; for (auto& child : child_list) { size.height = std::max(size.height, child.region.size.height); }
	UpdateLayout(begin);
}

ListLayoutAuto<Horizontal>::child_type ListLayoutAuto<Horizontal>::ExtractChild(size_t index) {
	child_ptr ptr = _ListLayoutAuto_Base::ExtractChild(index);
	EraseChild(index, 1);
	return ptr;
}

std::vector<ListLayoutAuto<Horizontal>::child_type> ListLayoutAuto<Horizontal>::ExtractChild(size_t begin, size_t count) {
	std::vector<child_type> ptr_list = _ListLayoutAuto_Base::ExtractChild(begin, count);
	EraseChild(begin, count);
	return ptr_list;
}

void ListLayoutAuto<Horizontal>::UpdateLayout(size_t index) {
	size.width = index == 0 ? (Empty() ? 0.0f : -gap) : child_list[index - 1].region.right();
	for (index; index < child_list.size(); index++) {
		size.width += gap;
		child_list[index].region.point.x = size.width;
		size.width += child_list[index].region.size.width;
	}
	SizeUpdated(size);
}

size_t ListLayoutAuto<Horizontal>::HitTestIndex(Point point) {
	static auto cmp = [](const ChildInfo& item, float offset) { return offset >= item.region.point.x; };
	return std::lower_bound(child_list.begin(), child_list.end(), point.x, cmp) - child_list.begin() - 1;
}

void ListLayoutAuto<Horizontal>::OnChildSizeUpdate(WndObject& child, Size child_size) {
	size_t index = GetChildIndex(child); ChildInfo& info = child_list[index];
	Size child_size_old = info.region.size; float height_old = size.height;
	info.region.size = child_size;
	if (size.height == child_size_old.height && child_size.height < size.height) {
		size.height = 0.0f; for (auto& child : child_list) { size.height = std::max(size.height, child.region.size.height); }
	} else if (size.height < child_size.height) {
		size.height = child_size.height;
	}
	if (child_size.width != child_size_old.width) {
		UpdateLayout(index + 1);
	} else if (size.height != height_old) {
		SizeUpdated(size);
	} else if (child_size.height != child_size_old.height) {
		Redraw(Rect(info.region.point, Size(child_size.width, std::max(child_size.height, child_size_old.height))));
	}
}

void ListLayoutAuto<Horizontal>::OnDraw(FigureQueue& figure_queue, Rect draw_region) {
	draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
	size_t index_begin = HitTestIndex(draw_region.LeftTop()), index_end = HitTestIndex(draw_region.RightBottom());
	for (size_t index = index_begin; index <= index_end; ++index) { DrawChild(GetChild(index), GetChildRegion(index), figure_queue, draw_region); }
}

ref_ptr<WndObject> ListLayoutAuto<Horizontal>::HitTest(MouseMsg& msg) {
	size_t index = HitTestIndex(msg.point); if (index >= Length()) { return this; }
	Rect child_region = GetChildRegion(index); if (!child_region.Contains(msg.point)) { return this; }
	msg.point -= child_region.point - point_zero;
	return &GetChild(index);
}


} // namespace ViewDesign

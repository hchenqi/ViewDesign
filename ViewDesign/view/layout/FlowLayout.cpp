#include "FlowLayout.h"

#include <algorithm>


namespace ViewDesign {


FlowLayout::FlowLayout(float row_height, float column_gap, float row_gap) :
	row_height(row_height), column_gap(column_gap), row_gap(row_gap), row_list({ 0 }) {}

Rect FlowLayout::GetChildRegion(ViewBase& child) const {
	auto [row, column] = GetChildData(child); const ChildInfo& info = child_list[row_list[row] + column];
	return Rect(Point(info.offset, GetRowOffset(row)), Size(info.width, row_height));
}

float FlowLayout::GetRowOffset(uint row) const { return row * (row_height + row_gap); }

uint FlowLayout::GetRowNumber() const { return (uint)row_list.size() - 1; }

uint FlowLayout::HitTestRow(float y) const { return (uint)floorf(y / (row_height + row_gap)); }

auto FlowLayout::HitTestColumn(row_index row, float x) const {
	static auto cmp = [](const ChildInfo& item, float offset) { return offset >= item.offset; };
	return std::lower_bound(child_list.begin() + row_list[row], child_list.begin() + row_list[row + 1], x, cmp) - 1;
}

void FlowLayout::AppendChild(child_type child) {
	RegisterChild(child);
	child_index child_index = (uint)child_list.size();
	ChildInfo& info = child_list.emplace_back(std::move(child));
	info.width = UpdateChildSizeRef(info.child, Size(length_min, row_height)).width;
	if (UpdateLayout(child_index)) { SizeUpdated(size); }
}

bool FlowLayout::UpdateLayout(child_index child_index) {
	uint row = 0, column = 0; float offset = 0;
	if (child_index > 0) {
		ChildInfo& prev = child_list[child_index - 1]; std::tie(row, column) = GetChildData(prev.child);
		column++; offset = prev.offset + prev.width + column_gap;
	}
	uint row_begin = row;
	row_list.erase(row_list.begin() + row + 1, row_list.end());
	for (; child_index < child_list.size(); ++child_index) {
		ChildInfo& info = child_list[child_index];
		if (offset > 0 && offset + info.width > size.width) { row_list.push_back(child_index); row++; column = 0; offset = 0; }
		info.offset = offset;
		SetChildData(info.child, { row, column });
		offset += info.width + column_gap; column++;
	}
	row_list.push_back((uint)child_list.size());
	float height = child_list.empty() ? 0 : GetRowOffset(row) + row_height;
	if (size.height == height) { Redraw(Rect(Point(0, GetRowOffset(row_begin)), size_max)); return false; }
	size.height = height; return true;
}

Transform FlowLayout::GetChildTransform(ViewBase& child) const {
	return GetChildRegion(child).point - point_zero;
}

Size FlowLayout::OnSizeRefUpdate(Size size_ref) {
	if (size.width != size_ref.width) {
		size.width = size_ref.width;
		UpdateLayout(0);
	}
	return size;
}

void FlowLayout::OnChildSizeUpdate(ViewBase& child, Size child_size) {
	auto [row, column] = GetChildData(child);
	child_index child_index = row_list[row] + column;
	ChildInfo& info = child_list[child_index];
	if (info.width != child_size.width) {
		info.width = child_size.width;
		if (UpdateLayout(child_index)) { SizeUpdated(size); }
	}
}

void FlowLayout::OnChildRedraw(ViewBase& child, Rect child_redraw_region) {
	Rect child_region = GetChildRegion(child);
	Redraw(child_region.Intersect(child_redraw_region + (child_region.point - point_zero)));
}

void FlowLayout::OnDraw(Canvas& canvas, Rect draw_region) {
	draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
	row_index row_begin = HitTestRow(draw_region.top()), row_end = HitTestRow(ceilf(draw_region.bottom()) - 1.0f);
	for (row_index row = row_begin; row <= row_end; ++row) {
		float row_offset = GetRowOffset(row);
		auto it_begin = HitTestColumn(row, draw_region.left()), it_end = HitTestColumn(row, ceilf(draw_region.right()) - 1.0f);
		for (auto it = it_begin; it <= it_end; ++it) {
			Rect child_region(Point(it->offset, row_offset), Size(it->width, row_height));
			DrawChild(it->child, child_region, canvas, draw_region);
		}
	}
}

ref_ptr<ViewBase> FlowLayout::HitTest(MouseEvent& event) {
	if (!Rect(point_zero, size).Contains(event.point)) { return nullptr; }
	row_index row = HitTestRow(event.point.y); if (event.point.y - GetRowOffset(row) >= row_height) { return this; }
	auto it = HitTestColumn(row, event.point.x); if (event.point.x - it->offset >= it->width) { return this; }
	event.point.x -= it->offset; event.point.y -= GetRowOffset(row);
	return it->child;
}


} // namespace ViewDesign

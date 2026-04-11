#include "OverlapLayout.h"
#include "../common/reversion_wrapper.h"


namespace ViewDesign {


void OverlapLayout::AddChild(frame_ptr frame) {
	RegisterChild(*frame);
	UpdateOverlapFrameChildSizeRef(*frame, size);
	Redraw(frame_list.emplace_back(std::move(frame))->region);
}

OverlapLayout::frame_ptr OverlapLayout::RemoveChild(OverlapFrame& frame) {
	auto it = std::find_if(frame_list.begin(), frame_list.end(), [&](const frame_ptr& ptr) { return ptr.get() == &frame; });
	if (it == frame_list.end()) { throw std::invalid_argument("invalid OverlapFrame reference"); }
	frame_ptr ptr = std::move(*it); frame_list.erase(it);
	Redraw(ptr->region);
	UnregisterChild(*ptr);
	return ptr;
}

void OverlapLayout::BringForward(OverlapFrame& frame) {
	auto it = std::find_if(frame_list.begin(), frame_list.end(), [&](const frame_ptr& ptr) { return ptr.get() == &frame; });
	if (it == frame_list.end()) { throw std::invalid_argument("invalid OverlapFrame reference"); }
	auto next = std::next(it);
	if (next == frame_list.end()) { return; }
	std::swap(*it, *next);
	Redraw((*it)->region.Intersect((*next)->region));
}

void OverlapLayout::BringToFront(OverlapFrame& frame) {
	auto it = std::find_if(frame_list.begin(), frame_list.end(), [&](const frame_ptr& ptr) { return ptr.get() == &frame; });
	if (it == frame_list.end()) { throw std::invalid_argument("invalid OverlapFrame reference"); }
	frame_list.splice(frame_list.end(), frame_list, it);
	Redraw((*it)->region);
}

void OverlapLayout::SendBackward(OverlapFrame& frame) {
	auto it = std::find_if(frame_list.begin(), frame_list.end(), [&](const frame_ptr& ptr) { return ptr.get() == &frame; });
	if (it == frame_list.end()) { throw std::invalid_argument("invalid OverlapFrame reference"); }
	if (it == frame_list.begin()) { return; }
	auto prev = std::prev(it);
	std::swap(*it, *prev);
	Redraw((*it)->region.Intersect((*prev)->region));
}

void OverlapLayout::SendToBack(OverlapFrame& frame) {
	auto it = std::find_if(frame_list.begin(), frame_list.end(), [&](const frame_ptr& ptr) { return ptr.get() == &frame; });
	if (it == frame_list.end()) { throw std::invalid_argument("invalid OverlapFrame reference"); }
	frame_list.splice(frame_list.begin(), frame_list, it);
	Redraw((*it)->region);
}

Transform OverlapLayout::GetChildTransform(WndObject& child) const {
	return AsFrame(child).region.point - point_zero;
}

Size OverlapLayout::OnSizeRefUpdate(Size size_ref) {
	size = size_ref;
	for (auto& frame : frame_list) { UpdateOverlapFrameChildSizeRef(*frame, size); }
	return size;
}

void OverlapLayout::OnOverlapFrameChildRegionUpdate(WndObject& child, Rect child_region) {
	OverlapFrame& frame = AsFrame(child);
	Rect redraw_region = frame.region.Union(child_region);
	frame.region = child_region;
	Redraw(redraw_region);
}

void OverlapLayout::OnChildRedraw(WndObject& child, Rect child_redraw_region) {
	Redraw(child_redraw_region + (AsFrame(child).region.point - point_zero));
}

void OverlapLayout::OnDraw(FigureQueue& figure_queue, Rect draw_region) {
	for (auto& frame : frame_list) {
		DrawChild(*frame, frame->region, figure_queue, draw_region);
	}
}

ref_ptr<WndObject> OverlapLayout::HitTest(MouseMsg& msg) {
	MouseMsg msg_copy = msg;
	for (auto& frame : reverse(frame_list)) {
		if (frame->region.Contains(msg.point)) {
			msg.point -= frame->region.point - point_zero;
			if (ref_ptr<WndObject> res = HitTestChild(*frame.get(), msg)) {
				return res;
			}
			msg = msg_copy;
		}
	}
	return this;
}


} // namespace ViewDesign

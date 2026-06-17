#include "ViewDesign/view/ViewBase.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {


ViewBase::~ViewBase() {
	if (HasParent()) {
		GetParent().UnregisterChild(*this);
	}
	desktop.Get().ReleaseView(*this);
}

ViewBase& ViewBase::GetDirectChild(ViewBase& descendent) const {
	ref_ptr<ViewBase> child = &descendent;
	for (ref_ptr<ViewBase> parent = child->parent; parent != this; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("invalid descendent view"); }
	}
	return *child;
}

Point ViewBase::ConvertDescendentPoint(ViewBase& descendent, Point point) const {
	ref_ptr<ViewBase> child = &descendent;
	for (ref_ptr<ViewBase> parent = child->parent; parent != this; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("invalid descendent view"); }
		point = parent->ConvertChildPoint(*child, point);
	}
	return ConvertChildPoint(*child, point);
}

Point ViewBase::ConvertDescendentPoint(Point point, ViewBase& descendent) const {
	ref_ptr<ViewBase> parent = descendent.parent;
	if (parent != this) {
		if (parent == nullptr) { throw std::invalid_argument("invalid descendent view"); }
		point = ConvertDescendentPoint(point, *parent);
		return parent->ConvertChildPoint(point, descendent);
	} else {
		return ConvertChildPoint(point, descendent);
	}
}

void ViewBase::DrawChild(ViewBase& child, Point child_offset, Canvas& canvas, Rect draw_region) {
	VerifyChild(child);
	if (draw_region.IsEmpty()) { return; }
	Vector offset = child_offset - point_zero; draw_region -= offset;
	canvas.Offset(offset, [&]() { child.OnDraw(canvas, draw_region); });
}

void ViewBase::DrawChild(ViewBase& child, Rect child_region, Canvas& canvas, Rect draw_region) {
	VerifyChild(child);
	draw_region = draw_region.Intersect(child_region); if (draw_region.IsEmpty()) { return; }
	Vector offset = child_region.point - point_zero; draw_region -= offset;
	canvas.Group(offset, draw_region, [&]() { child.OnDraw(canvas, draw_region); });
}


void ViewBase::SetCapture() { desktop.Get().SetCapture(*this); }

void ViewBase::ReleaseCapture() { desktop.Get().ReleaseCapture(*this); }

void ViewBase::SetFocus() { desktop.Get().SetFocus(*this); }

void ViewBase::ReleaseFocus() { desktop.Get().ReleaseFocus(*this); }


} // namespace ViewDesign

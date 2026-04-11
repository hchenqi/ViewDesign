#include "ViewBase.h"
#include "Desktop.h"


namespace ViewDesign {


ViewBase::~ViewBase() {
	if (HasParent()) {
		GetParent().UnregisterChild(*this);
	}
	desktop.ReleaseWindow(*this);
}

ViewBase& ViewBase::GetDirectChild(ViewBase& descendent) const {
	ref_ptr<ViewBase> child = &descendent;
	while (child->parent != this) {
		if (child->parent == nullptr) { throw std::invalid_argument("invalid descendent window"); }
		child = child->parent;
	}
	return *child;
}

Transform ViewBase::GetDescendentTransform(ViewBase& descendent) const {
	Transform transform = Transform::Identity();
	for (ref_ptr<ViewBase> child = &descendent, parent = descendent.parent; child != this; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("invalid descendent window"); }
		transform = transform * parent->GetChildTransform(*child);
	}
	return transform;
}

Point ViewBase::ConvertDescendentPoint(ViewBase& descendent, Point point) const {
	for (ref_ptr<ViewBase> child = &descendent, parent = descendent.parent; child != this; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("invalid descendent window"); }
		point *= parent->GetChildTransform(*child);
	}
	return point;
}

void ViewBase::DrawChild(ViewBase& child, Point child_offset, FigureQueue& figure_queue, Rect draw_region) {
	VerifyChild(child);
	if (draw_region.IsEmpty()) { return; }
	Vector offset = child_offset - point_zero; draw_region -= offset;
	figure_queue.Offset(offset, [&]() { child.OnDraw(figure_queue, draw_region); });
}

void ViewBase::DrawChild(ViewBase& child, Rect child_region, FigureQueue& figure_queue, Rect draw_region) {
	VerifyChild(child);
	draw_region = draw_region.Intersect(child_region); if (draw_region.IsEmpty()) { return; }
	Vector offset = child_region.point - point_zero; draw_region -= offset;
	figure_queue.Group(offset, draw_region, [&]() { child.OnDraw(figure_queue, draw_region); });
}


void ViewBase::SetCapture() { desktop.SetCapture(*this); }

void ViewBase::ReleaseCapture() { desktop.ReleaseCapture(*this); }

void ViewBase::SetFocus() { desktop.SetFocus(*this); }

void ViewBase::ReleaseFocus() { desktop.ReleaseFocus(*this); }


} // namespace ViewDesign

#pragma once

#include "ViewDesign/view/view_traits.h"


namespace ViewDesign {


class ViewFrame : public ViewBase {
public:
	ViewFrame(view_ptr<> child) : child(std::move(child)) { RegisterChild(this->child); }
	virtual ~ViewFrame() override {}

	// child
protected:
	view_ptr<> child;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { DrawChild(child, point_zero, canvas, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


class ViewFrameRef : public ViewBase {
public:
	ViewFrameRef(view_ref<> child) : child(child) { RegisterChild(child); }
	virtual ~ViewFrameRef() override { UnregisterChild(child); }

	// child
protected:
	view_ref<> child;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { DrawChild(child, point_zero, canvas, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/view/view_traits.h"


namespace ViewDesign {


class ReferenceFrame : public ViewBase {
public:
	ReferenceFrame(view_ref_any child) : child(child) { RegisterChild(child); }
	virtual ~ReferenceFrame() override { UnregisterChild(child); }

	// child
protected:
	view_ref_any child;

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

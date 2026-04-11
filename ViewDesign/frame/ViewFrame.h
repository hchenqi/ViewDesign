#pragma once

#include "../window/view_traits.h"


namespace ViewDesign {


class ViewFrame : public ViewBase {
public:
	ViewFrame(child_ptr<> child) : child(std::move(child)) { RegisterChild(this->child); }
	virtual ~ViewFrame() override {}

	// child
protected:
	child_ptr<> child;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override { DrawChild(child, point_zero, figure_queue, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


class ViewFrameRef : public ViewBase {
public:
	ViewFrameRef(child_ref<> child) : child(child) { RegisterChild(child); }
	virtual ~ViewFrameRef() override { UnregisterChild(child); }

	// child
protected:
	child_ref<> child;

	// layout
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return Transform::Identity(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override { DrawChild(child, point_zero, figure_queue, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


} // namespace ViewDesign

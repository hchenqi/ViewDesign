#pragma once

#include "ViewDesign/view/view_traits.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class ReferenceFrame : public ViewBase {
public:
	using child_type = view_ref<WidthTrait, HeightTrait>;

public:
	ReferenceFrame(child_type child) : child(child) { RegisterChild(child); }
	virtual ~ReferenceFrame() override { UnregisterChild(child); }

	// child
protected:
	child_type child;

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { DrawChild(child, point_zero, canvas, draw_region); }
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


template<class T>
ReferenceFrame(T) -> ReferenceFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

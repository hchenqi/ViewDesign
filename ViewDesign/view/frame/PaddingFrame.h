#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


using Padding = Margin;


template<class WidthTrait, class HeightTrait>
class PaddingFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	PaddingFrame(Padding padding, child_type child) : ViewFrame(std::move(child)), padding(padding) {}

	// style
protected:
	Padding padding;

	// layout
protected:
	Size size;
	Size child_size;
protected:
	Vector GetChildOffset() const { return Vector(padding.left, padding.top); }
	Rect GetChildRegion() const { return Rect(point_zero + GetChildOffset(), child_size); }
protected:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const override { return point + GetChildOffset(); }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const override { return point - GetChildOffset(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size = Extend(child_size = UpdateChildSizeRef(child, Extend(size_ref, -padding)), padding); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { SizeUpdated(size = Extend(this->child_size = child_size, padding)); }

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { return DrawChild(child, point_zero + GetChildOffset(), canvas, draw_region); }
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region + GetChildOffset()); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point -= GetChildOffset(); return ViewFrame::HitTest(event); }
};


template<class T>
PaddingFrame(Padding, T) -> PaddingFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

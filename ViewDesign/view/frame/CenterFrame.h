#pragma once

#include "../ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait = Fixed, class HeightTrait = Fixed>
class CenterFrame;


class _CenterFrame_Base : public ViewFrame {
protected:
	_CenterFrame_Base(view_ptr<> child) : ViewFrame(std::move(child)) {}

	// layout
protected:
	Size size;
	Size child_size;
protected:
	Vector GetChildOffset() const { return Vector((size.width - child_size.width) / 2, (size.height - child_size.height) / 2); }
	Rect GetChildRegion() const { return Rect(point_zero + GetChildOffset(), child_size); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return GetChildOffset(); }

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region + GetChildOffset()); }
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { return DrawChild(child, point_zero + GetChildOffset(), canvas, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point -= GetChildOffset(); return ViewFrame::HitTest(event); }
};


template<>
class CenterFrame<Fixed, Fixed> : public _CenterFrame_Base, public SizeTrait<Fixed, Fixed> {
public:
	CenterFrame(view_ptr<> child) : _CenterFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { child_size = UpdateChildSizeRef(child, size = size_ref); return size; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { this->child_size = child_size; Redraw(region_infinite); }
};


template<class HeightTrait> requires (!IsFixed<HeightTrait>)
class CenterFrame<Fixed, HeightTrait> : public _CenterFrame_Base, public SizeTrait<Fixed, HeightTrait> {
public:
	CenterFrame(view_ptr<Relative, HeightTrait> child) : _CenterFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_size = UpdateChildSizeRef(this->child, size_ref);
		return size = Size(size_ref.width, child_size.height);
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		this->child_size = child_size;
		if (size.height != child_size.height) {
			size.height = child_size.height; SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};


template<class WidthTrait> requires (!IsFixed<WidthTrait>)
class CenterFrame<WidthTrait, Fixed> : public _CenterFrame_Base, public SizeTrait<WidthTrait, Fixed> {
public:
	CenterFrame(view_ptr<WidthTrait, Relative> child) : _CenterFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_size = UpdateChildSizeRef(this->child, size_ref);
		return size = Size(child_size.width, size_ref.height);
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		this->child_size = child_size;
		if (size.width != child_size.width) {
			size.width = child_size.width; SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};


} // namespace ViewDesign

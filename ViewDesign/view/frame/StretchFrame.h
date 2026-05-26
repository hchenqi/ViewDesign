#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class StretchFrame;


class _StretchFrame_Base : public ViewFrame {
protected:
	_StretchFrame_Base(view_ptr_any child) : ViewFrame(std::move(child)) {}

	// layout
protected:
	Size size;
	Size child_size;
protected:
	Scale GetCurrentScale() const { return child_size.IsEmpty() ? scale_identity : Scale(size.width / child_size.width, size.height / child_size.height); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return GetCurrentScale(); }

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region * GetCurrentScale());
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.Group(GetCurrentScale(), rect_infinite, [&]() {
			DrawChild(child, point_zero, canvas, draw_region / GetCurrentScale());
		});
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point /= GetCurrentScale(); return ViewFrame::HitTest(event); }
};


template<>
class StretchFrame<Fixed, Fixed> : public _StretchFrame_Base, public SizeTrait<Fixed, Fixed> {
public:
	StretchFrame(view_ptr_any child) : _StretchFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { child_size = UpdateChildSizeRef(child, size = size_ref); return size; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { this->child_size = child_size; Redraw(rect_infinite); }
};


template<class HeightTrait> requires (!IsFixed<HeightTrait>)
class StretchFrame<Fixed, HeightTrait> : public _StretchFrame_Base, public SizeTrait<Fixed, HeightTrait> {
public:
	StretchFrame(view_ptr<Relative, HeightTrait> child) : _StretchFrame_Base(std::move(child)) {}
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
			Redraw(rect_infinite);
		}
	}
};


template<class WidthTrait> requires (!IsFixed<WidthTrait>)
class StretchFrame<WidthTrait, Fixed> : public _StretchFrame_Base, public SizeTrait<WidthTrait, Fixed> {
public:
	StretchFrame(view_ptr<WidthTrait, Relative> child) : _StretchFrame_Base(std::move(child)) {}
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
			Redraw(rect_infinite);
		}
	}
};


} // namespace ViewDesign

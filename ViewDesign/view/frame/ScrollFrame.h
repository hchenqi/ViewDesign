#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


template<class WidthTrait = Bounded, class HeightTrait = Bounded>
class ScrollFrame;


class _ScrollFrame_Base : public ViewFrame {
protected:
	_ScrollFrame_Base(view_ptr_any child) : ViewFrame(std::move(child)) {}

	// layout
protected:
	Size size_ref;
	Size child_size;
	Size size;
	Point offset = point_zero;
protected:
	static Size CalculateSize(Size size_ref, Size child_size) {
		return Size(std::min(size_ref.width, child_size.width), std::min(size_ref.height, child_size.height));
	}
	static Point ClampOffset(Size size, Size child_size, Point offset) {
		return Point(std::clamp(offset.x, 0.0f, child_size.width - size.width), std::clamp(offset.y, 0.0f, child_size.height - size.height));
	}
protected:
	Vector GetChildOffset() const { return point_zero - offset; }
	Rect GetChildRegion() const { return Rect(point_zero + GetChildOffset(), child_size); }
protected:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const override { return point + GetChildOffset(); }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const override { return point - GetChildOffset(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_size = UpdateChildSizeRef(child, this->size_ref = size_ref);
		size = CalculateSize(this->size_ref, child_size);
		offset = ClampOffset(size, child_size, offset);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (this->child_size != child_size) {
			this->child_size = child_size;
			Size size = CalculateSize(size_ref, this->child_size);
			offset = ClampOffset(size, this->child_size, offset);
			if (this->size != size) {
				SizeUpdated(this->size = size);
			} else {
				Redraw(rect_infinite);
			}
		}
	}

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		DrawChild(child, point_zero + GetChildOffset(), canvas, draw_region);
	}
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region + GetChildOffset());
	}

	// scroll
public:
	void ScrollTo(Point offset) {
		offset = ClampOffset(size, child_size, offset);
		if (this->offset != offset) {
			this->offset = offset;
			Redraw(rect_infinite);
		}
	}
	void Scroll(Vector offset) {
		if (offset != vector_zero) {
			ScrollTo(this->offset + offset);
		}
	}
	void ScrollIntoView(Point point) {
		point = Clamp(point, Rect(point_zero, child_size));
		Scroll(point - Clamp(point, Rect(offset, size)));
	}
	void ScrollIntoView(Rect rect) {
		rect = Clamp(rect, Rect(point_zero, child_size));
		Scroll(rect.point - Clamp(rect, Rect(offset, size)).point);
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point -= GetChildOffset(); return ViewFrame::HitTest(event); }
};


template<>
class ScrollFrame<Bounded, Bounded> : public _ScrollFrame_Base, public SizeTrait<Bounded, Bounded> {
public:
	ScrollFrame(view_ptr<Relative, Relative> child) : _ScrollFrame_Base(std::move(child)) {}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (!event.ctrl && (event.type == MouseEvent::WheelVertical || event.type == MouseEvent::WheelHorizontal)) { return this; }
		return _ScrollFrame_Base::HitTest(event);
	}
protected:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::WheelVertical: Scroll(Vector(0, -(float)event.wheel_delta)); break;
		case MouseEvent::WheelHorizontal: Scroll(Vector((float)event.wheel_delta, 0)); break;
		}
	}
};


template<class WidthTrait> requires (!IsBounded<WidthTrait>)
class ScrollFrame<WidthTrait, Bounded> : public _ScrollFrame_Base, public SizeTrait<WidthTrait, Bounded> {
public:
	using child_type = view_ptr<WidthTrait, Relative>;

public:
	ScrollFrame(child_type child) : _ScrollFrame_Base(std::move(child)) {}

	// scroll
public:
	using _ScrollFrame_Base::ScrollTo;
	using _ScrollFrame_Base::Scroll;
	using _ScrollFrame_Base::ScrollIntoView;
public:
	void ScrollTo(float offset) { ScrollTo(Point(0.0f, offset)); }
	void Scroll(float offset) { Scroll(Vector(0.0f, offset)); }
	void ScrollIntoView(float y) { ScrollIntoView(Point(0.0f, y)); }
	void ScrollIntoView(Interval interval) { ScrollIntoView(Rect(Interval(0.0f, size.width), interval)); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (!event.ctrl && event.type == MouseEvent::WheelVertical) { return this; }
		return _ScrollFrame_Base::HitTest(event);
	}
protected:
	virtual void OnMouseEvent(MouseEvent event) override {
		Scroll(-(float)event.wheel_delta);
	}
};


template<class HeightTrait> requires (!IsBounded<HeightTrait>)
class ScrollFrame<Bounded, HeightTrait> : public _ScrollFrame_Base, public SizeTrait<Bounded, HeightTrait> {
public:
	using child_type = view_ptr<Auto, HeightTrait>;

public:
	ScrollFrame(child_type child) : _ScrollFrame_Base(std::move(child)) {}

	// scroll
public:
	using _ScrollFrame_Base::ScrollTo;
	using _ScrollFrame_Base::Scroll;
	using _ScrollFrame_Base::ScrollIntoView;
public:
	void ScrollTo(float offset) { ScrollTo(Point(offset, 0.0f)); }
	void Scroll(float offset) { Scroll(Vector(offset, 0.0f)); }
	void ScrollIntoView(float x) { ScrollIntoView(Point(x, 0.0f)); }
	void ScrollIntoView(Interval interval) { ScrollIntoView(Rect(interval, Interval(0.0f, size.height))); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (!event.ctrl && event.type == MouseEvent::WheelHorizontal) { return this; }
		return _ScrollFrame_Base::HitTest(event);
	}
protected:
	virtual void OnMouseEvent(MouseEvent event) override {
		Scroll((float)event.wheel_delta);
	}
};


} // namespace ViewDesign

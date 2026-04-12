#pragma once

#include "../ViewFrame.h"
#include "../../geometry/clamp.h"


namespace ViewDesign {


template<class Direction>
class ScrollFrame;


class _ScrollFrame_Base : public ViewFrame, public SizeTrait<Fixed, Fixed> {
protected:
	_ScrollFrame_Base(view_ptr<> child) : ViewFrame(std::move(child)) {}

	// layout
protected:
	Size size;
	Size child_size;
	Point frame_offset;
protected:
	Size GetFrameSize() const { return size; }
	Size GetChildSize() const { return child_size; }
	Point GetFrameOffset() const { return frame_offset; }
	Vector GetChildOffset() const { return point_zero - frame_offset; }
	Rect GetChildRegion() const { return Rect(point_zero + GetChildOffset(), child_size); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return GetChildOffset(); }

	// scrolling
protected:
	Point ClampFrameOffset(Point offset) const {
		offset.x = child_size.width <= size.width ? 0.0f : clamp(offset.x, Interval(0.0f, child_size.width - size.width));
		offset.y = child_size.height <= size.height ? 0.0f : clamp(offset.y, Interval(0.0f, child_size.height - size.height));
		return offset;
	}
public:
	void ScrollTo(Point offset) {
		offset = ClampFrameOffset(offset);
		if (frame_offset != offset) {
			frame_offset = offset;
			Redraw(region_infinite);
		}
	}
	void Scroll(Vector offset) {
		if (offset != vector_zero) {
			ScrollTo(frame_offset + offset);
		}
	}
	void ScrollIntoView(Point point) {
		point = clamp(point, Rect(point_zero, child_size));
		Scroll(point - clamp(point, Rect(frame_offset, size)));
	}
	void ScrollIntoView(Rect rect) {
		rect = clamp(rect, Rect(point_zero, child_size));
		Scroll(rect.point - clamp(rect, Rect(frame_offset, size)).point);
	}

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region + GetChildOffset());
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		DrawChild(child, point_zero + GetChildOffset(), canvas, draw_region);
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point -= GetChildOffset(); return ViewFrame::HitTest(event); }
};


template<>
class ScrollFrame<Bidirectional> : public _ScrollFrame_Base {
public:
	ScrollFrame(view_ptr<Auto, Auto> child) : _ScrollFrame_Base(std::move(child)) {
		child_size = UpdateChildSizeRef(this->child, size_empty);
	}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size != size_ref) {
			size = size_ref;
			frame_offset = ClampFrameOffset(frame_offset);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (this->child_size != child_size) {
			this->child_size = child_size;
			frame_offset = ClampFrameOffset(frame_offset);
			Redraw(region_infinite);
		}
	}

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
		case MouseEvent::WheelHorizontal: Scroll(Vector(-(float)event.wheel_delta, 0)); break;
		}
	}
};


template<>
class ScrollFrame<Vertical> : public _ScrollFrame_Base {
public:
	ScrollFrame(view_ptr<Fixed, Auto> child) : _ScrollFrame_Base(std::move(child)) {}

	// layout
public:
	float GetFrameLength() const { return _ScrollFrame_Base::GetFrameSize().height; }
	float GetChildLength() const { return _ScrollFrame_Base::GetChildSize().height; }
	float GetFrameOffset() const { return _ScrollFrame_Base::GetFrameOffset().y; }

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

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size != size_ref) {
			if (size.width != size_ref.width) {
				child_size = Size(size_ref.width, UpdateChildSizeRef(child, Size(size_ref.width, length_max)).height);
			}
			size = size_ref;
			frame_offset = ClampFrameOffset(frame_offset);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (this->child_size.height != child_size.height) {
			this->child_size.height = child_size.height;
			frame_offset = ClampFrameOffset(frame_offset);
			Redraw(region_infinite);
		}
	}

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


template<>
class ScrollFrame<Horizontal> : public _ScrollFrame_Base {
public:
	ScrollFrame(view_ptr<Auto, Fixed> child) : _ScrollFrame_Base(std::move(child)) {}

	// layout
public:
	float GetFrameLength() const { return _ScrollFrame_Base::GetFrameSize().width; }
	float GetChildLength() const { return _ScrollFrame_Base::GetChildSize().width; }
	float GetFrameOffset() const { return _ScrollFrame_Base::GetFrameOffset().x; }

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

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (size != size_ref) {
			if (size.height != size_ref.height) {
				child_size = Size(UpdateChildSizeRef(child, Size(length_max, size_ref.height)).width, size_ref.height);
			}
			size = size_ref;
			frame_offset = ClampFrameOffset(frame_offset);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (this->child_size.width != child_size.width) {
			this->child_size.width = child_size.width;
			frame_offset = ClampFrameOffset(frame_offset);
			Redraw(region_infinite);
		}
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (!event.ctrl && event.type == MouseEvent::WheelHorizontal) { return this; }
		return _ScrollFrame_Base::HitTest(event);
	}
protected:
	virtual void OnMouseEvent(MouseEvent event) override {
		Scroll(-(float)event.wheel_delta);
	}
};


template<class WidthTrait, class HeightTrait>
struct deduce_scroll_frame_direction;

template<>
struct deduce_scroll_frame_direction<Auto, Auto> {
	using type = Bidirectional;
};

template<>
struct deduce_scroll_frame_direction<Fixed, Auto> {
	using type = Vertical;
};

template<>
struct deduce_scroll_frame_direction<Auto, Fixed> {
	using type = Horizontal;
};


template<class T>
ScrollFrame(T) -> ScrollFrame<typename deduce_scroll_frame_direction<extract_width_trait<T>, extract_height_trait<T>>::type>;


} // namespace ViewDesign

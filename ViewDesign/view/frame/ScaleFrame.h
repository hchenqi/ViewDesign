#pragma once

#include "ViewDesign/view/ViewFrame.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class ScaleFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	ScaleFrame(Scale scale, child_type child) : ViewFrame(std::move(child)), scale(scale) {}
	ScaleFrame(child_type child) : ScaleFrame(1.0f, std::move(child)) {}

	// style
protected:
	Scale scale;
protected:
	void SetScale(Scale scale) {
		if (this->scale != scale) {
			this->scale = scale;
			SizeUpdated(UpdateChildSizeRef(child, size_ref / scale) * scale);
			Redraw(rect_infinite);
		}
	}

	// layout
protected:
	Size size_ref;
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return scale; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, (this->size_ref = size_ref) / scale) * scale; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { SizeUpdated(child_size * scale); }

	// drawing
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region * scale);
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.Group(scale, rect_infinite, [&]() {
			DrawChild(child, point_zero, canvas, draw_region / scale);
		});
	}

	// event
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.ctrl && event.type == MouseEvent::WheelVertical) { return this; }
		event.point /= scale;
		return ViewFrame::HitTest(event);
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		SetScale(scale * Scale(powf(1.1f, event.wheel_delta / 120.0f)));
	}
};


template<class T>
ScaleFrame(Scale, T) -> ScaleFrame<extract_width_trait<T>, extract_height_trait<T>>;

template<class T>
ScaleFrame(T) -> ScaleFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

#pragma once

#include "../ViewFrame.h"
#include "../../geometry/helper.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class ScaleFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	ScaleFrame(Scale scale, child_type child) : ViewFrame(std::move(child)), scale(scale) {}

	// style
protected:
	Scale scale;
protected:
	void SetScale(Scale scale) {
		if (this->scale != scale) {
			this->scale = scale;
			SizeUpdated(UpdateChildSizeRef(child, size_ref * scale.Invert()) * scale);
			Redraw(region_infinite);
		}
	}

	// layout
protected:
	Size size_ref;
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return scale; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return UpdateChildSizeRef(child, (this->size_ref = size_ref) * scale.Invert()) * scale; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { SizeUpdated(child_size * scale); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region * scale);
	}
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		figure_queue.Group(scale, region_infinite, [&]() {
			DrawChild(child, point_zero, figure_queue, draw_region * scale.Invert());
		});
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point *= scale.Invert(); return ViewFrame::HitTest(event); }
};


template<class T>
ScaleFrame(Scale, T) -> ScaleFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

#pragma once

#include "ViewDesign/view/ViewFrame.h"
#include "ViewDesign/style/border_style.h"
#include "ViewDesign/drawing/shape.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class InnerBorderFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	InnerBorderFrame(Border border, child_type child) : ViewFrame(std::move(child)), border(border) {}

	// style
protected:
	Border border;
public:
	void SetBorder(Border border) { this->border = border; Redraw(region_infinite); }

	// layout
protected:
	Size size;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size = UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { SizeUpdated(size = child_size); }

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		DrawChild(child, point_zero, canvas, draw_region);
		if (border._width > 0.0f && border._color.IsVisible()) {
			if (border._radius > 0.0f) {
				canvas.draw(point_zero, new RoundedRectangle(size, border._radius, border._width, border._color));
			} else {
				canvas.draw(point_zero, new Rectangle(size, border._width, border._color));
			}
		}
	}
};


template<class T>
InnerBorderFrame(Border, T) -> InnerBorderFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

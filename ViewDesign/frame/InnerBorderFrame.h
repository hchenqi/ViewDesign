#pragma once

#include "ViewFrame.h"
#include "../style/border_style.h"
#include "../figure/shape.h"


namespace ViewDesign {


template<class WidthType, class HeightType>
class InnerBorderFrame : public ViewFrame, public LayoutType<WidthType, HeightType> {
public:
	using child_type = view_ptr<WidthType, HeightType>;

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

	// paint
protected:
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		DrawChild(child, point_zero, figure_queue, draw_region);
		if (border._width > 0.0f && border._color.IsVisible()) {
			if (border._radius > 0.0f) {
				figure_queue.add(point_zero, new RoundedRectangle(size, border._radius, border._width, border._color));
			} else {
				figure_queue.add(point_zero, new Rectangle(size, border._width, border._color));
			}
		}
	}
};


template<class T>
InnerBorderFrame(Border, T) -> InnerBorderFrame<extract_width_type<T>, extract_height_type<T>>;


} // namespace ViewDesign

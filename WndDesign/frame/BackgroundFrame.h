#pragma once

#include "WndFrame.h"
#include "../figure/shape.h"


namespace ViewDesign {


template<class WidthType, class HeightType>
class BackgroundFrame : public WndFrame, public LayoutType<WidthType, HeightType> {
public:
	using child_type = child_ptr<WidthType, HeightType>;

public:
	BackgroundFrame(Color background, child_type child) : WndFrame(std::move(child)), background(background) {}

	// style
protected:
	Color background;
protected:
	void SetBackground(Color background) {
		if (this->background != background) {
			this->background = background;
			Redraw(region_infinite);
		}
	}

	// paint
protected:
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		figure_queue.add(draw_region.point, new Rectangle(draw_region.size, background));
		WndFrame::OnDraw(figure_queue, draw_region);
	}
};


template<class T>
BackgroundFrame(Color, T) -> BackgroundFrame<extract_width_type<T>, extract_height_type<T>>;


} // namespace ViewDesign

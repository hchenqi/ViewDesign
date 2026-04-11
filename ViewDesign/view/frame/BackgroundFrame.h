#pragma once

#include "../ViewFrame.h"
#include "../../figure/shape.h"


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class BackgroundFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	BackgroundFrame(Color background, child_type child) : ViewFrame(std::move(child)), background(background) {}

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
		ViewFrame::OnDraw(figure_queue, draw_region);
	}
};


template<class T>
BackgroundFrame(Color, T) -> BackgroundFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

#pragma once

#include "../window/WndObject.h"
#include "../figure/shape.h"


namespace ViewDesign {


template<class Wnd, Color color = Color::White> requires std::is_base_of_v<WndObject, Wnd>
class SolidColorBackground : public Wnd {
protected:
	using Base = SolidColorBackground;

public:
	using Wnd::Wnd;

	// style
protected:
	Color background = color;

	// paint
protected:
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		figure_queue.add(draw_region.point, new Rectangle(draw_region.size, background));
		Wnd::OnDraw(figure_queue, draw_region);
	}
};


} // namespace ViewDesign

#pragma once

#include "../ViewBase.h"
#include "../../figure/shape.h"


namespace ViewDesign {


template<class View, Color color = Color::White> requires std::is_base_of_v<ViewBase, View>
class SolidColorBackground : public View {
protected:
	using Base = SolidColorBackground;

public:
	using View::View;

	// style
protected:
	Color background = color;

	// paint
protected:
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
		figure_queue.add(draw_region.point, new Rectangle(draw_region.size, background));
		View::OnDraw(figure_queue, draw_region);
	}
};


} // namespace ViewDesign

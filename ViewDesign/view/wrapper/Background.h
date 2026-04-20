#pragma once

#include "../ViewBase.h"
#include "../../drawing/shape.h"


namespace ViewDesign {


template<class View, Color color = Color::White> requires std::derived_from<View, ViewBase>
class SolidColorBackground : public View {
protected:
	using Base = SolidColorBackground;

public:
	using View::View;

	// style
protected:
	Color background = color;

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.draw(draw_region.point, new Rectangle(draw_region.size, background));
		View::OnDraw(canvas, draw_region);
	}
};


} // namespace ViewDesign

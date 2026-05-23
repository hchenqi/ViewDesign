#pragma once

#include "ViewDesign/view/ViewBase.h"
#include "ViewDesign/view/figure/shape.h"


namespace ViewDesign {


template<Color color, class View> requires std::derived_from<View, ViewBase>
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


template<class View>
using DefaultBackground = SolidColorBackground<Color::White, View>;


} // namespace ViewDesign

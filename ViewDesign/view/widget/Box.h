#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"
#include "ViewDesign/view/style/border.h"
#include "ViewDesign/view/figure/shape.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


using Padding = Margin;


template<class WidthTrait, class HeightTrait>
class Box : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	Box(Margin margin, Border border, Color background, Padding padding, child_type child) : ViewFrame(std::move(child)), margin(margin), border(border), background(background), padding(padding) {}

	// style
protected:
	Margin margin;
	Border border;
	Color background;
	Padding padding;
public:
	void SetBorderColor(Color color) { if (border._color != color) { border._color = color; Redraw(rect_infinite); } }
	void SetBackground(Color background) { if (this->background != background) { this->background = background; Redraw(rect_infinite); } }

	// layout
protected:
	Size size;
	Size child_size;
protected:
	Rect GetBorderRegion() const { return Extend(Rect(point_zero, size), -margin); }
	Rect GetBackgroundRegion() const { return Extend(GetBorderRegion(), -border._width); }
	Margin GetChildMargin() const { return Margin(margin.left + border._width + padding.left, margin.top + border._width + padding.top, margin.right + border._width + padding.right, margin.bottom + border._width + padding.bottom); }
	Vector GetChildOffset() const { return Vector(margin.left + border._width + padding.left, margin.top + border._width + padding.top); }
	Rect GetChildRegion() const { return Rect(point_zero + GetChildOffset(), child_size); }
protected:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const override { return point + GetChildOffset(); }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const override { return point - GetChildOffset(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { Margin child_margin = GetChildMargin(); return size = Extend(child_size = UpdateChildSizeRef(child, Extend(size_ref, -child_margin)), child_margin); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { SizeUpdated(size = Extend(this->child_size = child_size, GetChildMargin())); }

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		if (background.IsVisible()) {
			Rect background_region = GetBackgroundRegion();
			if (border._radius > 0.0f) {
				canvas.draw(background_region.point, new RoundedRectangle(background_region.size, border._radius, background));
			} else {
				canvas.draw(background_region.point, new Rectangle(background_region.size, background));
			}
		}
		DrawChild(child, point_zero + GetChildOffset(), canvas, draw_region);
		if (border._width > 0.0f && border._color.IsVisible()) {
			Rect border_region = GetBorderRegion();
			if (border._radius > 0.0f) {
				canvas.draw(border_region.point, new RoundedRectangle(border_region.size, border._radius, border._width, border._color));
			} else {
				canvas.draw(border_region.point, new Rectangle(border_region.size, border._width, border._color));
			}
		}
	}
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		Redraw(child_redraw_region + GetChildOffset());
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (PointInRoundedRectangle(event.point, GetBackgroundRegion(), border._radius)) {
			event.point -= GetChildOffset();
			return ViewFrame::HitTest(event);
		}
		return nullptr;
	}
};


template<class T>
Box(Margin, Border, Color, Padding, T) -> Box<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign

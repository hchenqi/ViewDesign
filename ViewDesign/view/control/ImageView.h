#pragma once

#include "ViewDesign/view/view_traits.h"
#include "ViewDesign/view/figure/image.h"


namespace ViewDesign {


class ImageView : public ViewBase, public SizeTrait<Auto, Auto> {
public:
	ImageView(const u16string& filename) : image(filename) {}
protected:
	Image image;
protected:
	Size GetSize() { return image.GetSize(); }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return GetSize(); }
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, GetSize())); if (draw_region.IsEmpty()) { return; }
		canvas.draw(draw_region.point, new ImageFigure(image, draw_region));
	}
};


class ImageRepeatView : public ViewBase, public SizeTrait<Fixed, Fixed> {
public:
	ImageRepeatView(const u16string& filename, Point offset = point_zero) : image(filename), offset(offset - point_zero) {}
protected:
	Image image;
	Vector offset;
protected:
	Size size;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size = size_ref; }
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.draw(draw_region.point, new ImageRepeatFigure(image, Rect(draw_region.point + offset, draw_region.size)));
	}
};


} // namespace ViewDesign

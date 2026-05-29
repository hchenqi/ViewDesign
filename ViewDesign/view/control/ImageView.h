#pragma once

#include "ViewDesign/view/view_traits.h"
#include "ViewDesign/view/figure/image.h"
#include "ViewDesign/common/holder.h"


namespace ViewDesign {

namespace Stateful {


class ImageView : public ViewBase, public SizeTrait<Auto, Auto> {
public:
	ImageView(const Image& image) : image(image) {}
protected:
	const Image& image;
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
	ImageRepeatView(const Image& image, Point offset = point_zero) : image(image), offset(offset - point_zero) {}
protected:
	const Image& image;
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


} // namespace Stateful


class ImageView : protected Holder<Image>, public Stateful::ImageView {
public:
	ImageView(const u16string& filename) : Holder(filename), Stateful::ImageView(value) {}
};


class ImageRepeatView : protected Holder<Image>, public Stateful::ImageRepeatView {
public:
	ImageRepeatView(const u16string& filename, Point offset = point_zero) : Holder(filename), Stateful::ImageRepeatView(value, offset) {}
};


} // namespace ViewDesign

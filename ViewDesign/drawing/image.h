#pragma once

#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/drawing/figure.h"
#include "ViewDesign/common/unicode.h"


namespace ViewDesign {

struct ImageSource;


class Image : Uncopyable {
private:
	friend struct ImageFigure;
	owner_ptr<ImageSource> source;
	Size size;
	mutable Bitmap bitmap;
public:
	Image(u16string file_name);
	Image(void* address, size_t size);
	~Image();
	Size GetSize() const { return size; }
	void CreateBitmap() const;
};


struct ImageFigure : Figure {
	const Image& image;
	Rect region;
	uchar opacity;

	ImageFigure(const Image& image, Rect region = region_infinite, uchar opacity = 0xFF) :
		image(image), region(Rect(point_zero, image.size).Intersect(region)), opacity(opacity) {
		image.CreateBitmap();
	}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


struct ImageRepeatFigure : Figure {
	const Image& image;
	Rect region;
	uchar opacity;

	ImageRepeatFigure(const Image& image, Rect region, uchar opacity = 0xFF) : image(image), region(region), opacity(opacity) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

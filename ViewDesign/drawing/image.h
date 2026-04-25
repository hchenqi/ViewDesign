#pragma once

#include "ViewDesign/drawing/texture.h"
#include "ViewDesign/drawing/figure.h"
#include "ViewDesign/common/unicode.h"


namespace ViewDesign {

struct ImageSource;


class Image : Uncopyable {
public:
	Image(u16string file_name);
	Image(void* address, size_t size);
	~Image();
private:
	owner_ptr<ImageSource> source;
	Size size;
	mutable Texture texture;
public:
	Size GetSize() const { return size; }
	const Texture& GetTexture() const;
};


struct ImageFigure : Figure {
	const Image& image;
	Rect region;
	uchar opacity;

	ImageFigure(const Image& image, Rect region = region_infinite, uchar opacity = 0xFF) : image(image), region(Rect(point_zero, image.GetSize()).Intersect(region)), opacity(opacity) {}

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

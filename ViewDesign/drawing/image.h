#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/drawing/figure.h"

#include <stdexcept>


namespace ViewDesign {


class Image : Uncopyable {
public:
	Image(u16string file_name);
	Image(void* address, size_t size);
	~Image();
protected:
	Handle source;
	Size size;
	mutable Handle texture = nullptr;
public:
	Size GetSize() const { return size; }
	Handle GetTexture() const { if (texture == nullptr) { throw std::invalid_argument("Image: texture not created"); } return texture; }
public:
	void CreateTexture() const;
	void DestroyTexture() const;
};


struct ImageFigure : Figure {
	const Image& image;
	Rect region;
	float opacity;

	ImageFigure(const Image& image, Rect region = region_infinite, float opacity = 1.0f) : image(image), region(Rect(point_zero, image.GetSize()).Intersect(region)), opacity(opacity) { image.CreateTexture(); }

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

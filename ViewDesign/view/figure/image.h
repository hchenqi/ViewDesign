#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/common/unicode.h"
#include "ViewDesign/geometry/sizeu.h"
#include "ViewDesign/drawing/figure.h"

#include <stdexcept>


namespace ViewDesign {


class Image : Uncopyable {
public:
	Image(const u16string& filename);
	Image(const void* buffer, size_t size);
	~Image();
protected:
	Handle source;
	SizeU size;
	mutable Handle texture = nullptr;
public:
	SizeU GetSize() const { return size; }
	Handle GetTexture() const { if (texture == nullptr) { throw std::logic_error("Image: texture not created"); } return texture; }
public:
	void CreateTexture() const;
	void DestroyTexture() const;
};


struct ImageFigure : Figure {
	const Image& image;
	Rect region;
	float opacity;

	ImageFigure(const Image& image, Rect region = rect_infinite, float opacity = 1.0f) : image(image), region(region.Intersect(RectI(point_i_zero, image.GetSize()))), opacity(opacity) { image.CreateTexture(); }

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


struct ImageRepeatFigure : Figure {
	const Image& image;
	Rect region;
	float opacity;

	ImageRepeatFigure(const Image& image, Rect region, float opacity = 1.0f) : image(image), region(region), opacity(opacity) { image.CreateTexture(); }

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

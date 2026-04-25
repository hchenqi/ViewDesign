#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/drawing/texture.h"
#include "ViewDesign/drawing/figure.h"


namespace ViewDesign {


class Bitmap : Uncopyable {
public:
	Bitmap(PixelBuffer pixel_buffer) : pixel_buffer(std::move(pixel_buffer)) {}
private:
	PixelBuffer pixel_buffer;
	mutable Texture texture;
public:
	Size GetSize() const { auto [width, height] = pixel_buffer.Size(); return Size(width, height); }
	const Texture& GetTexture() const;
};


struct BitmapFigure : Figure {
	const Bitmap& bitmap;
	Rect region;
	uchar opacity;

	BitmapFigure(const Bitmap& bitmap, Rect region = region_infinite, uchar opacity = 0xFF) : bitmap(bitmap), region(Rect(point_zero, bitmap.GetSize()).Intersect(region)), opacity(opacity) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

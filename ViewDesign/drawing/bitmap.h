#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/drawing/figure.h"


namespace ViewDesign {


class Bitmap : Uncopyable {
public:
	Bitmap() : pixel_buffer() {}
	Bitmap(PixelBuffer pixel_buffer) : pixel_buffer(std::move(pixel_buffer)) {}
	~Bitmap() { DestroyTexture(); }
protected:
	PixelBuffer pixel_buffer;
	mutable Handle texture = nullptr;
public:
	Size GetSize() const { auto [width, height] = pixel_buffer.Size(); return Size(width, height); }
	Handle GetTexture() const { if (texture == nullptr) { throw std::invalid_argument("Bitmap: texture not created"); } return texture; }
public:
	void CreateTexture() const;
	void DestroyTexture() const;
public:
	void Set(PixelBuffer pixel_buffer) { DestroyTexture(); this->pixel_buffer = std::move(pixel_buffer); }
	void Update(auto f) { DestroyTexture(); f(pixel_buffer); }
};


struct BitmapFigure : Figure {
	const Bitmap& bitmap;
	Rect region;
	float opacity;

	BitmapFigure(const Bitmap& bitmap, Rect region, float opacity) : bitmap(bitmap), region(Rect(point_zero, bitmap.GetSize()).Intersect(region)), opacity(opacity) { bitmap.CreateTexture(); }
	BitmapFigure(const Bitmap& bitmap, float opacity) : BitmapFigure(bitmap, region_infinite, opacity) {}
	BitmapFigure(const Bitmap& bitmap, Rect region) : BitmapFigure(bitmap, region, 1.0f) {}
	BitmapFigure(const Bitmap& bitmap) : BitmapFigure(bitmap, region_infinite, 1.0f) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

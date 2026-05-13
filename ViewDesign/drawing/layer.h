#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/geometry/sizeu.h"
#include "ViewDesign/drawing/figure.h"

#include <stdexcept>


namespace ViewDesign {

class Canvas;


class Layer : Uncopyable {
public:
	Layer() {}
	~Layer() { DestroyFramebuffer(); }
private:
	SizeU size = size_u_empty;
	Handle framebuffer = nullptr;
public:
	SizeU GetSize() const { return size; }
	bool HasFramebuffer() const { return framebuffer != nullptr; }
	Handle GetFramebuffer() const { if (!HasFramebuffer()) { throw std::invalid_argument("Layer: framebuffer not created"); } return framebuffer; }
public:
	void SetFramebuffer(SizeU size, Handle framebuffer) { DestroyFramebuffer(); this->size = size; this->framebuffer = framebuffer; }
	void CreateFramebuffer(SizeU size);
	void DestroyFramebuffer();
public:
	void RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region);
};


struct LayerFigure : Figure {
	const Layer& layer;
	Rect region;
	Size size;
	float opacity;

	LayerFigure(const Layer& layer, Rect region, Size size, float opacity) : layer(layer), region(region.Intersect(RectI(point_i_zero, layer.GetSize()))), size(size), opacity(opacity) {}
	LayerFigure(const Layer& layer, Rect region, float opacity) : LayerFigure(layer, region, region.size, opacity) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/common/type.h"
#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/drawing/figure.h"

#include <stdexcept>


namespace ViewDesign {

class Canvas;


class Layer : Uncopyable {
public:
	Layer() {}
	~Layer() { DestroyFramebuffer(); }
private:
	Size size = size_empty;
	Handle framebuffer = nullptr;
public:
	Size GetSize() const { return size; }
	bool HasFramebuffer() const { return framebuffer != nullptr; }
	Handle GetFramebuffer() const { if (!HasFramebuffer()) { throw std::invalid_argument("Layer: framebuffer not created"); } return framebuffer; }
public:
	void SetFramebuffer(Size size, Handle framebuffer) { DestroyFramebuffer(); this->size = size; this->framebuffer = framebuffer; }
	void CreateFramebuffer(Size size);
	void DestroyFramebuffer();
public:
	void RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region);
};


struct LayerFigure : Figure {
	const Layer& layer;
	Rect region;
	Size size;
	float opacity;

	LayerFigure(const Layer& layer, Rect region, Size size, float opacity) : layer(layer), region(Rect(point_zero, layer.GetSize()).Intersect(region)), size(size), opacity(opacity) {}
	LayerFigure(const Layer& layer, Rect region, float opacity) : LayerFigure(layer, region, region.size, opacity) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

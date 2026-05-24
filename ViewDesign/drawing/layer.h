#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/geometry/sizeu.h"
#include "ViewDesign/drawing/canvas.h"

#include <stdexcept>


namespace ViewDesign {


class Layer : Uncopyable {
public:
	Layer() {}
	~Layer() { Destroy(); }
private:
	Handle framebuffer = nullptr;
	SizeU size;
public:
	bool Empty() const { return framebuffer == nullptr; }
	Handle GetFramebuffer() const { if (Empty()) { throw std::logic_error("Layer: framebuffer not created"); } return framebuffer; }
	SizeU GetSize() const { if (Empty()) { throw std::logic_error("Layer: framebuffer not created"); } return size; }
public:
	void Create(SizeU size);
	void Destroy();
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

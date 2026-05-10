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
	~Layer() { DestroyTexture(); }
private:
	Size size = size_empty;
	Handle texture = nullptr;
public:
	Size GetSize() const { return size; }
	bool IsEmpty() const { return texture == nullptr; }
	Handle GetTexture() const { if (IsEmpty()) { throw std::invalid_argument("Layer: texture not created"); } return texture; }
public:
	void SetTexture(Size size, Handle texture) { DestroyTexture(); this->size = size; this->texture = texture; }
	void CreateTexture(Size size);
	void DestroyTexture();
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

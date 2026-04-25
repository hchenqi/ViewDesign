#pragma once

#include "ViewDesign/drawing/texture.h"
#include "ViewDesign/drawing/figure.h"


namespace ViewDesign {

class Canvas;


class Layer : Uncopyable {
public:
	Layer() {}
	~Layer() {}
protected:
	Texture texture;
public:
	bool IsEmpty() const { return texture.IsEmpty(); }
	const Texture& GetTexture() const { return texture; }
public:
	void Create(Size size);
	void Destroy() { texture.Destroy(); }
public:
	void DrawCanvas(const Canvas& canvas, Vector offset, Rect clip_region);
};


struct LayerFigure : Figure {
	const Layer& layer;
	Rect region;
	Size size;
	uchar opacity;

	LayerFigure(const Layer& layer, Rect region, uchar opacity = 0xFF) : LayerFigure(layer, region, region.size, opacity) {}
	LayerFigure(const Layer& layer, Rect region, Size size, uchar opacity = 0xFF) : layer(layer), region(region), size(size), opacity(opacity) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


} // namespace ViewDesign

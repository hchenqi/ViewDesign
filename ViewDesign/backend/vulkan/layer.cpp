#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/drawing/canvas.h"


namespace ViewDesign {


void Layer::Create(Size size) {}

void Layer::RenderCanvas(const Canvas& canvas, Vector offset, Rect clip_region) {}


void LayerFigure::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

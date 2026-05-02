#include "ViewDesign/view/frame/LayerFrame.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


Size _LayerFrame_Base::OnSizeRefUpdate(Size size_ref) {
	Size child_size = UpdateChildSizeRef(child, size_ref);
	if (size != child_size) { size = child_size; layer.Destroy(); }
	return size;
}

void _LayerFrame_Base::OnChildSizeUpdate(ViewBase& child, Size child_size) {
	if (size != child_size) { size = child_size; layer.Destroy(); SizeUpdated(size); }
}

void _LayerFrame_Base::OnChildRedraw(ViewBase& child, Rect child_redraw_region) {
	child_redraw_region = child_redraw_region.Intersect(Rect(point_zero, size));
	if (!layer.IsEmpty()) { invalid_region.Union(child_redraw_region * scale); }
	Redraw(child_redraw_region);
}

void _LayerFrame_Base::OnDraw(Canvas& canvas, Rect draw_region) {
	draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
	if (Scale current_scale = canvas.GetTransform().GetScale(); scale != current_scale) { scale = current_scale; layer.Destroy(); }
	Rect composite_region = draw_region * scale, redraw_region = composite_region;
	if (layer.IsEmpty()) {
		Size layer_size = size * scale;
		layer.Create(layer_size);
		invalid_region.Set(Rect(point_zero, layer_size));
	} else {
		Region render_region(redraw_region); render_region.Intersect(invalid_region);
		redraw_region = render_region.GetBoundingRect();
	}
	if (!redraw_region.IsEmpty()) {
		Canvas canvas;
		canvas.Group(scale, region_infinite, [&]() {DrawChild(child, point_zero, canvas, redraw_region * scale.Invert()); });
		layer.RenderCanvas(canvas, vector_zero, redraw_region);
		invalid_region.Sub(redraw_region);
	}
	canvas.draw(draw_region.point, new LayerFigure(layer, composite_region, draw_region.size, opacity));
}


} // namespace ViewDesign

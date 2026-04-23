#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


void Window::SetSize(Size size) {
	if (region.size != size) {
		region.size = size;
		UpdateChildSizeRef(child, size * scale.Invert());
		ResizeLayer();
	}
}

std::pair<Size, Rect> Window::GetMinMaxRegion(Size size_ref) {
	auto [size_min, size_max] = CalculateMinMaxSize(size_ref * scale.Invert());
	return { size_min * scale, Rect(point_zero, size_max * scale) };
}

void Window::Destroy() {
	desktop.RemoveWindow(*this);
}

void Window::ResizeLayer() {
	layer.Resize(region.size);
	invalid_region.Clear();
	Redraw(region_infinite);
}

void Window::RecreateLayer() {
	layer.Create(hwnd, region.size);
	invalid_region.Clear();
	Redraw(region_infinite);
}


} // namespace ViewDesign

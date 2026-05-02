#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/system/window.h"
#include "ViewDesign/geometry/helper.h"


namespace ViewDesign {


Window::Window(const u16string& title, view_ptr<> child) : ViewFrame(std::move(child)), handle(CreateWindow(*this, title)), scale(GetWindowScale(handle)) {}

Window::~Window() {
	layer.Destroy();
	Close();
}

void Window::SetTitle(const u16string& title) {
	SetWindowTitle(handle, title);
}

std::pair<Size, Rect> Window::GetMinMaxRegion(Size size_ref) {
	auto [size_min, size_max] = CalculateMinMaxSize(size_ref * scale.Invert());
	return { size_min * scale, Rect(point_zero, size_max * scale) };
}

void Window::InitializeRegion(Size size_ref) {
	region = Round(OnWindowSizeRefUpdate(size_ref * scale.Invert()) * scale);
	SetWindowRegion(handle, region);
	RecreateLayer();
}

void Window::SetSize(Size size) {
	if (region.size != size) {
		region.size = size;
		UpdateChildSizeRef(child, size * scale.Invert());
		ResizeLayer();
	}
}

void Window::WindowRegionUpdated(Rect region) {
	if (!HasParent()) { return; }
	region = Round(region * scale);
	if (this->region.size != region.size) {
		this->region.size = region.size;
		ResizeLayer();
	}
	SetWindowRegion(handle, region);
}

void Window::Show() { ShowWindow(handle); }
void Window::Hide() { HideWindow(handle); }
void Window::Minimize() { MinimizeWindow(handle); }
void Window::Maximize() { MaximizeWindow(handle); }
void Window::Restore() { RestoreWindow(handle); }
void Window::Close() { CloseWindow(handle); }

void Window::ResizeLayer() {
	layer.Resize(region.size);
	Redraw(region_infinite);
}

void Window::RecreateLayer() {
	layer.Create(handle, region.size);
	Redraw(region_infinite);
}

void Window::Redraw(Rect redraw_region) {
	redraw_region = RoundUp((redraw_region * scale).Intersect(Rect(point_zero, region.size)));
	layer.Redraw(redraw_region);
	RedrawWindowRegion(handle, redraw_region);
}

void Window::OnDraw() {
	Rect draw_region = layer.GetInvalidRegion(); if (draw_region.IsEmpty()) { return; }
	Canvas canvas;
	canvas.Group(scale, region_infinite, [&]() { ViewFrame::OnDraw(canvas, draw_region * scale.Invert()); });
	layer.RenderCanvas(canvas);
	layer.Present();
}


} // namespace ViewDesign

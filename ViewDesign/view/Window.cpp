#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/system/window.h"


namespace ViewDesign {


Window::Window(const u16string& title, view_ptr<> child) : ViewFrame(std::move(child)), handle(CreateWindow(*this, title)), region(rect_i_empty), scale(GetWindowScale(handle)) {}

Window::~Window() { desktop.Get().ReleaseWindow(*this); layer.Destroy(); DestroyWindow(handle); handle = nullptr; }

void Window::SetTitle(const u16string& title) {
	SetWindowTitle(handle, title);
}

std::pair<SizeU, RectI> Window::GetMinMaxRegion(SizeU desktop_size) {
	auto [size_min, size_max] = CalculateMinMaxSize(desktop_size / scale);
	return { Round(size_min * scale), RectI(point_i_zero, Round(size_max * scale)) };
}

void Window::InitializeRegion(SizeU desktop_size) {
	region = Round(OnWindowSizeRefUpdate(desktop_size / scale) * scale);
	SetWindowRegion(handle, region);
	RecreateLayer();
}

void Window::SetSize(SizeU size) {
	if (region.size != size) {
		region.size = size;
		UpdateChildSizeRef(child, size / scale);
		ResizeLayer();
	}
}

void Window::WindowRegionUpdated(Rect region) {
	if (!HasParent()) { return; }
	RectI region_new = Round(region * scale);
	if (this->region.size != region_new.size) {
		this->region.size = region_new.size;
		ResizeLayer();
	}
	SetWindowRegion(handle, region_new);
}

void Window::Show() { ShowWindow(handle); }
void Window::Hide() { HideWindow(handle); }
void Window::Minimize() { MinimizeWindow(handle); }
void Window::Maximize() { MaximizeWindow(handle); }
void Window::Restore() { RestoreWindow(handle); }
void Window::Close() { CloseWindow(handle); }

void Window::ResizeLayer() {
	layer.Resize(region.size);
	Redraw(rect_infinite);
}

void Window::RecreateLayer() {
	layer.Create(handle, region.size);
	Redraw(rect_infinite);
}

void Window::Redraw(Rect redraw_region) {
	RectI redraw_region_i = RoundUp((redraw_region * scale).Intersect(Rect(point_zero, region.size)));
	layer.Redraw(redraw_region_i);
	RedrawWindowRegion(handle, redraw_region_i);
}

void Window::OnDraw() {
	Rect draw_region = layer.GetInvalidRegion(); if (draw_region.IsEmpty()) { return; }
	layer.RenderBegin();
	Canvas canvas;
	canvas.Group(scale, rect_infinite, [&]() { ViewFrame::OnDraw(canvas, draw_region / scale); });
	layer.RenderEnd(canvas);
}


} // namespace ViewDesign

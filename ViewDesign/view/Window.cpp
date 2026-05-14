#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/system/window.h"


namespace ViewDesign {


Window::Window(const u16string& title, view_ptr<> child) : ViewFrame(std::move(child)), layer(CreateWindow(*this, title)), point(point_i_zero), scale(GetWindowScale(GetHandle())) {}

Window::~Window() { desktop.Get().ReleaseWindow(*this); DestroyWindow(layer.DestroyWindow()); }

void Window::SetTitle(const u16string& title) { SetWindowTitle(GetHandle(), title); }

std::pair<SizeU, RectI> Window::GetMinMaxRegion(SizeU desktop_size) {
	auto [size_min, size_max] = CalculateMinMaxSize(desktop_size / scale);
	return { Round(size_min * scale), RectI(point_i_zero, Round(size_max * scale)) };
}

void Window::InitializeRegion(SizeU desktop_size) {
	RectI region = Round(OnWindowSizeRefUpdate(desktop_size / scale) * scale);
	SetWindowRegion(GetHandle(), region);
	if (GetSize() != region.size) { layer.Resize(region.size); }
	point = region.point;
}

void Window::SetSize(SizeU size) {
	if (GetSize() != size) {
		layer.Resize(size);
		UpdateChildSizeRef(child, size / scale);
	}
}

void Window::WindowRegionUpdated(Rect rect) {
	if (!HasParent()) { return; }
	RectI region = Round(rect * scale);
	SetWindowRegion(GetHandle(), region);
	if (GetSize() != region.size) { layer.Resize(region.size); }
	point = region.point;
}

void Window::Show() { ShowWindow(GetHandle()); }
void Window::Hide() { HideWindow(GetHandle()); }
void Window::Minimize() { MinimizeWindow(GetHandle()); }
void Window::Maximize() { MaximizeWindow(GetHandle()); }
void Window::Restore() { RestoreWindow(GetHandle()); }
void Window::Close() { CloseWindow(GetHandle()); }

void Window::Redraw(Rect rect) {
	RectI redraw_region = RoundUp(rect * scale).Intersect(RectI(point_i_zero, GetSize()));
	layer.Redraw(redraw_region);
	RedrawWindowRegion(GetHandle(), redraw_region);
}

void Window::OnDraw() {
	layer.Render([&](Rect draw_region) {
		Canvas canvas;
		canvas.Group(scale, rect_infinite, [&]() { ViewFrame::OnDraw(canvas, draw_region / scale); });
		return canvas;
	});
}


} // namespace ViewDesign

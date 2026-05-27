#include "ViewDesign/view/Window.h"
#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/system/window.h"


namespace ViewDesign {


Window::Window(Handle window, std::unique_ptr<ViewBase> child) : surface(window), child(std::move(child)), point(point_i_zero), scale(GetWindowScale(GetHandle())) { AttachWindow(GetHandle(), *this); RegisterChild(*this->child); }
Window::Window(const u16string& title, std::unique_ptr<ViewBase> child) : Window(CreateWindow(title), std::move(child)) {}
Window::~Window() { desktop.Get().ReleaseWindow(*this); DestroyWindow(surface.DestroyWindow()); }

void Window::SetTitle(const u16string& title) { SetWindowTitle(GetHandle(), title); }
void Window::RegionUpdated(Rect rect) { SetWindowRegion(GetHandle(), Round(rect * scale)); }

void Window::Show() { ShowWindow(GetHandle()); }
void Window::Hide() { HideWindow(GetHandle()); }
void Window::Minimize() { MinimizeWindow(GetHandle()); }
void Window::Maximize() { MaximizeWindow(GetHandle()); }
void Window::Restore() { RestoreWindow(GetHandle()); }
void Window::Close() { CloseWindow(GetHandle()); }

void Window::Draw() {
	surface.Render([&](Rect draw_region) {
		Canvas canvas;
		canvas.Group(scale, rect_infinite, [&]() { OnDraw(canvas, draw_region / scale); });
		return canvas;
	});
}

void Window::Redraw(Rect rect) {
	RectI redraw_region = RoundUp(rect * scale).Intersect(RectI(point_i_zero, GetSize()));
	surface.Redraw(redraw_region);
	RedrawWindowRegion(GetHandle(), redraw_region);
}


} // namespace ViewDesign

#include "ViewDesign/view/Window.h"
#include "ViewDesign/system/window.h"


namespace ViewDesign {


Window::Window(Handle window, view_ptr_any child) : surface(window), child(std::move(child)), point(point_i_zero), scale(GetWindowScale(GetHandle())) { AttachWindow(GetHandle(), *this); RegisterChild(*this->child); }
Window::Window(const u16string& title, view_ptr_any child) : Window(CreateWindow(title), std::move(child)) {}
Window::~Window() { DestroyWindow(surface.DestroyWindow()); }

void Window::SetTitle(const u16string& title) { SetWindowTitle(GetHandle(), title); }
void Window::SetIcon(const void* buffer, size_t size) { SetWindowIcon(GetHandle(), buffer, size); }
void Window::ClearIcon() { ClearWindowIcon(GetHandle()); }
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
		canvas.Group(scale, rect_infinite, [&] { OnDraw(canvas, draw_region / scale); });
		return canvas;
	});
}

void Window::Redraw(Rect rect) {
	RectI redraw_region = RoundUp(rect * scale).Intersect(RectI(point_i_zero, GetPixelSize()));
	surface.Redraw(redraw_region);
	RedrawWindowRegion(GetHandle(), redraw_region);
}


} // namespace ViewDesign

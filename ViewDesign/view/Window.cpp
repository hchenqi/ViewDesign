#include "Desktop.h"
#include "../drawing/window_layer.h"
#include "../geometry/helper.h"
#include "../system/win32_api.h"
#include "../system/d2d_api.h"
#include "../system/directx_resource.h"


namespace ViewDesign {

namespace {

inline float RoundWin32Length(float length) { length = floorf(length); return length < 14.0f ? 14.0f : length; }

} // namespace


Window::Window(std::wstring title, view_ptr<> child) : ViewFrame(std::move(child)) {
	hwnd = Win32::CreateWnd(region_empty, title);
	Win32::SetWndUserData(hwnd, this);
	scale = Scale(Win32::GetWndDpiScale(hwnd));
}

Window::~Window() {
	layer.Destroy();
	Win32::SetWndUserData(hwnd, nullptr);
	Win32::DestroyWnd(hwnd);
}

void Window::SetTitle(std::wstring title) { Win32::SetWndTitle(hwnd, title); }

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

void Window::InitializeRegion(Size size_ref) {
	region = OnWindowSizeRefUpdate(size_ref * scale.Invert()) * scale;
	region.size = Size(RoundWin32Length(region.size.width), RoundWin32Length(region.size.height));
	Win32::SetWndRegion(hwnd, region);
	RecreateLayer();
}

void Window::WindowRegionUpdated(Rect region) {
	if (!HasParent()) { return; }
	region *= scale;
	region.size = Size(RoundWin32Length(region.size.width), RoundWin32Length(region.size.height));
	if (this->region.size != region.size) {
		this->region.size = region.size;
		ResizeLayer();
	}
	Win32::SetWndRegion(hwnd, region);
}

void Window::Show() { Win32::ShowWnd(hwnd); }
void Window::Hide() { Win32::HideWnd(hwnd); }
void Window::SetForeground() { Win32::SetForegroundWnd(hwnd); }
void Window::Minimize() { Win32::MinimizeWnd(hwnd); }
void Window::Maximize() { Win32::MaximizeWnd(hwnd); }
void Window::Restore() { Win32::RestoreWnd(hwnd); }
void Window::Destroy() { desktop.RemoveWindow(*this); }

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

void Window::OnDraw() {
	Rect render_rect = invalid_region.GetBoundingRect(); if (render_rect.IsEmpty()) { return; }
	BeginDraw();
	Canvas canvas([&](Canvas& canvas) {
		canvas.Group(scale, region_infinite, [&]() {
			OnDraw(canvas, render_rect * scale.Invert());
		});
	});
	layer.DrawCanvas(canvas, vector_zero, render_rect);
	try {
		EndDraw();
	} catch (std::runtime_error&) {
		DirectXRecreateResource();
		return desktop.RecreateFrameLayer();
	}
	invalid_region.Clear();
	layer.Present(render_rect);
}

Point Window::GetDesktopCursorPosition() const {
	return Win32::GetCursorPos() * scale.Invert();
}

void Window::Redraw(Rect redraw_region) {
	redraw_region = Extend(redraw_region * scale, 0.5f).Intersect(Rect(point_zero, region.size));
	invalid_region.Union(redraw_region);
	Win32::InvalidateWndRegion(hwnd, redraw_region);
}


} // namespace ViewDesign

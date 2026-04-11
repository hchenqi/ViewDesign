#include "DesktopFrame.h"
#include "../window/desktop.h"
#include "../figure/desktop_layer.h"
#include "../geometry/helper.h"
#include "../system/win32_api.h"
#include "../system/d2d_api.h"
#include "../system/directx_resource.h"


namespace ViewDesign {

namespace {

inline float RoundWin32Length(float length) { length = floorf(length); return length < 14.0f ? 14.0f : length; }

} // namespace


DesktopFrame::DesktopFrame(std::wstring title, view_ptr<> child) : ViewFrame(std::move(child)) {
	hwnd = Win32::CreateWnd(region_empty, title);
	Win32::SetWndUserData(hwnd, this);
	scale = Scale(Win32::GetWndDpiScale(hwnd));
}

DesktopFrame::~DesktopFrame() {
	layer.Destroy();
	Win32::SetWndUserData(hwnd, nullptr);
	Win32::DestroyWnd(hwnd);
}

void DesktopFrame::SetTitle(std::wstring title) { Win32::SetWndTitle(hwnd, title); }

void DesktopFrame::SetSize(Size size) {
	if (region.size != size) {
		region.size = size;
		UpdateChildSizeRef(child, size * scale.Invert());
		ResizeLayer();
	}
}

std::pair<Size, Rect> DesktopFrame::GetMinMaxRegion(Size size_ref) {
	auto [size_min, size_max] = CalculateMinMaxSize(size_ref * scale.Invert());
	return { size_min * scale, Rect(point_zero, size_max * scale) };
}

void DesktopFrame::InitializeRegion(Size size_ref) {
	region = OnDesktopFrameSizeRefUpdate(size_ref * scale.Invert()) * scale;
	region.size = Size(RoundWin32Length(region.size.width), RoundWin32Length(region.size.height));
	Win32::SetWndRegion(hwnd, region);
	RecreateLayer();
}

void DesktopFrame::DesktopFrameRegionUpdated(Rect region) {
	if (!HasParent()) { return; }
	region *= scale;
	region.size = Size(RoundWin32Length(region.size.width), RoundWin32Length(region.size.height));
	if (this->region.size != region.size) {
		this->region.size = region.size;
		ResizeLayer();
	}
	Win32::SetWndRegion(hwnd, region);
}

void DesktopFrame::Show() { Win32::ShowWnd(hwnd); }
void DesktopFrame::Hide() { Win32::HideWnd(hwnd); }
void DesktopFrame::SetForeground() { Win32::SetForegroundWnd(hwnd); }
void DesktopFrame::Minimize() { Win32::MinimizeWnd(hwnd); }
void DesktopFrame::Maximize() { Win32::MaximizeWnd(hwnd); }
void DesktopFrame::Restore() { Win32::RestoreWnd(hwnd); }
void DesktopFrame::Destroy() { desktop.RemoveChild(*this); }

void DesktopFrame::ResizeLayer() {
	layer.Resize(region.size);
	invalid_region.Clear();
	Redraw(region_infinite);
}

void DesktopFrame::RecreateLayer() {
	layer.Create(hwnd, region.size);
	invalid_region.Clear();
	Redraw(region_infinite);
}

void DesktopFrame::OnDraw() {
	Rect render_rect = invalid_region.GetBoundingRect(); if (render_rect.IsEmpty()) { return; }
	BeginDraw();
	FigureQueue figure_queue([&](FigureQueue& figure_queue) {
		figure_queue.Group(scale, region_infinite, [&]() {
			OnDraw(figure_queue, render_rect * scale.Invert());
		});
	});
	layer.DrawFigureQueue(figure_queue, vector_zero, render_rect);
	try {
		EndDraw();
	} catch (std::runtime_error&) {
		DirectXRecreateResource();
		return desktop.RecreateFrameLayer();
	}
	invalid_region.Clear();
	layer.Present(render_rect);
}

Point DesktopFrame::GetDesktopCursorPosition() const {
	return Win32::GetCursorPos() * scale.Invert();
}

void DesktopFrame::Redraw(Rect redraw_region) {
	redraw_region = Extend(redraw_region * scale, 0.5f).Intersect(Rect(point_zero, region.size));
	invalid_region.Union(redraw_region);
	Win32::InvalidateWndRegion(hwnd, redraw_region);
}


} // namespace ViewDesign

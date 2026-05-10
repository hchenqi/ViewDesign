#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/win32/window.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "ViewDesign/platform/vulkan/window_surface.h"


namespace ViewDesign {

using namespace Win32;
using namespace Vulkan;


WindowLayer::WindowLayer() : window_surface(nullptr) {}

void WindowLayer::Create(Handle window, Size size) {
	Destroy();
	window_surface = new WindowSurface(InstanceContext::GetInstance().createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR({}, GetHInstance(), AsHWND(window))), size);
}

void WindowLayer::Destroy() {
	if (window_surface != nullptr) {
		delete static_cast<owner_ptr<WindowSurface>>(window_surface);
		window_surface = nullptr;
	}
}

void WindowLayer::Resize(Size size) {}

void WindowLayer::CreateTexture() {}

void WindowLayer::Redraw(Rect redraw_region) {
	invalid_region = invalid_region.Union(redraw_region);
}

void WindowLayer::RenderCanvas(const Canvas& canvas) {
	static_cast<ref_ptr<WindowSurface>>(window_surface)->DrawFrame([]() {

	});
	invalid_region = region_empty;
}


} // namespace ViewDesign

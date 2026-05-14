#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/win32/window.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "ViewDesign/platform/vulkan/surface.h"


namespace ViewDesign {

using namespace Win32;
using namespace Vulkan;


void WindowLayer::Resize(SizeU size) {
	Destroy();
	surface = new Surface(InstanceContext::GetInstance().createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR({}, GetHInstance(), AsHWND(window))), size);
}

void WindowLayer::Destroy() {
	if (surface != nullptr) {
		delete static_cast<owner_ptr<Surface>>(surface);
		surface = nullptr;
	}
}

void WindowLayer::RenderBegin() {
	static_cast<ref_ptr<Surface>>(surface)->RenderBegin();
}

void WindowLayer::RenderEnd(const Canvas& canvas) {
	static_cast<ref_ptr<Surface>>(surface)->Render([&]() {
		Layer::RenderCanvas(canvas, vector_zero, invalid_region);
	});
	invalid_region = rect_empty;
}


} // namespace ViewDesign

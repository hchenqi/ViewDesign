#include "ViewDesign/drawing/surface.h"
#include "ViewDesign/platform/win32/window.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "ViewDesign/platform/vulkan/surface.h"
#include "ViewDesign/platform/vulkan/canvas.h"


namespace ViewDesign {

using namespace Win32;
using VulkanSurface = Vulkan::Surface;


void Surface::Resize(SizeU size) {
	Destroy();
	surface = new VulkanSurface(Vulkan::InstanceContext::GetInstance().createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR({}, GetHInstance(), AsHWND(window))), size);
	invalid_region = RectI(point_i_zero, size);
}

void Surface::Destroy() {
	if (surface != nullptr) {
		delete static_cast<owner_ptr<VulkanSurface>>(surface);
		surface = nullptr;
	}
}

void Surface::RenderBegin() {
	static_cast<ref_ptr<VulkanSurface>>(surface)->RenderBegin();
}

void Surface::RenderEnd(const Canvas& canvas) {
	static_cast<ref_ptr<VulkanSurface>>(surface)->Render(invalid_region, [&](RenderTarget& target) { Vulkan::RenderCanvas(target, canvas, vector_zero, invalid_region);	});
}


} // namespace ViewDesign

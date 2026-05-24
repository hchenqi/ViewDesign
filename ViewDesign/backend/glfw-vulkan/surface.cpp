#include "ViewDesign/drawing/surface.h"

#include <ViewDesign/platform/vulkan/surface.h>
#include <ViewDesign/platform/glfw/window.h>


namespace ViewDesign {

using namespace GLFW;
using VulkanSurface = Vulkan::Surface;


void Surface::Resize(SizeU size) {
	Destroy();
	this->size = size;

	VkSurfaceKHR surface;
	VkResult result = glfwCreateWindowSurface(*Vulkan::InstanceContext::GetInstance(), AsGLFWWindow(window), nullptr, &surface);
	this->surface = new VulkanSurface(vk::raii::SurfaceKHR(Vulkan::InstanceContext::GetInstance(), surface), size);

	invalid_region = RectI(point_i_zero, size);
}

void Surface::Destroy() {
	if (surface != nullptr) {
		Vulkan::DeviceContext::Get().device.waitIdle();
		delete static_cast<owner_ptr<VulkanSurface>>(surface);
		surface = nullptr;
	}
}

void Surface::RenderBegin() {
	static_cast<ref_ptr<VulkanSurface>>(surface)->RenderBegin();
}

void Surface::RenderEnd(const Canvas& canvas) {
	static_cast<ref_ptr<VulkanSurface>>(surface)->Render(invalid_region, [&](RenderTarget& target) { ViewDesign::RenderCanvas(target, canvas, vector_zero, invalid_region);	});
}


} // namespace ViewDesign

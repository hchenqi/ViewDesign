#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/glad/framebuffer.h"
#include "ViewDesign/platform/glfw/window.h"


namespace ViewDesign {

using namespace GLFW;


WindowLayer::WindowLayer() : window(nullptr) {}

void WindowLayer::Create(Handle window, SizeU size) {
	this->window = window;
	CreateLayerFramebuffer(size);
}

void WindowLayer::Destroy() {}

void WindowLayer::Resize(SizeU size) {
	CreateLayerFramebuffer(size);
}

void WindowLayer::CreateLayerFramebuffer(SizeU size) {
	Layer::SetFramebuffer(size, nullptr);
	invalid_region = rect_i_empty;
}

void WindowLayer::Redraw(RectI redraw_region) {
	invalid_region = invalid_region.Union(redraw_region);
	invalid_region_front_buffer = invalid_region_front_buffer.Union(redraw_region);
}

void WindowLayer::RenderBegin() {
	glfwMakeContextCurrent(AsGLFWWindow(window));
}

void WindowLayer::RenderEnd(const Canvas& canvas) {
	Layer::RenderCanvas(canvas, vector_zero, invalid_region);

	glfwSwapBuffers(AsGLFWWindow(window));
	glfwMakeContextCurrent(nullptr);

	invalid_region = invalid_region_front_buffer;
	invalid_region_front_buffer = rect_i_empty;
}


} // namespace ViewDesign

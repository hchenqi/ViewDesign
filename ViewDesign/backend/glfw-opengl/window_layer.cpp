#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/glad/frame_buffer.h"
#include "ViewDesign/platform/glfw/window.h"


namespace ViewDesign {

using namespace GLFW;


WindowLayer::WindowLayer() : window(nullptr) {}

void WindowLayer::Create(Handle window, Size size) {
	this->window = window;
	CreateLayerTexture(size);
}

void WindowLayer::Destroy() {}

void WindowLayer::Resize(Size size) {
	CreateLayerTexture(size);
}

void WindowLayer::CreateLayerTexture(Size size) {
	Layer::SetTexture(size, nullptr);
	invalid_region = region_empty;
}

void WindowLayer::Redraw(Rect redraw_region) {
	invalid_region = invalid_region.Union(redraw_region);
	invalid_region_front_buffer = invalid_region_front_buffer.Union(redraw_region);
}

void WindowLayer::RenderCanvas(const Canvas& canvas) {
	glfwMakeContextCurrent(AsGLFWWindow(window));
	Layer::RenderCanvas(canvas, vector_zero, invalid_region);

	glfwSwapBuffers(AsGLFWWindow(window));
	glfwMakeContextCurrent(nullptr);

	invalid_region = invalid_region_front_buffer;
	invalid_region_front_buffer = region_empty;
}


} // namespace ViewDesign

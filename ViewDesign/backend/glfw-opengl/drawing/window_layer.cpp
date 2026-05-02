#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/glad/texture.h"
#include "ViewDesign/platform/glfw/window.h"


namespace ViewDesign {

using namespace GLFW;


WindowLayer::WindowLayer() : handle(nullptr) {
	texture.Set(new TextureResource());
}

void WindowLayer::Create(Handle handle, Size size) {
	this->handle = handle;
	this->size = size;
}

void WindowLayer::Destroy() {}

void WindowLayer::Resize(Size size) {
	this->size = size;
}

void WindowLayer::CreateTexture() {}

void WindowLayer::Redraw(Rect redraw_region) {
	invalid_region = invalid_region.Union(redraw_region);
	invalid_region_front_buffer = invalid_region_front_buffer.Union(redraw_region);
}

void WindowLayer::RenderCanvas(const Canvas& canvas) {
	glfwMakeContextCurrent(AsGLFWWindow(handle));
	Layer::RenderCanvas(canvas, vector_zero, invalid_region);
}

void WindowLayer::Present() {
	glfwSwapBuffers(AsGLFWWindow(handle));
	invalid_region = invalid_region_front_buffer;
	invalid_region_front_buffer = region_empty;
}


} // namespace ViewDesign

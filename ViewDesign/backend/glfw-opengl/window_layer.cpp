#include "ViewDesign/drawing/window_layer.h"
#include "ViewDesign/platform/glad/canvas.h"
#include "ViewDesign/platform/glfw/window.h"


namespace ViewDesign {

using namespace GLFW;
using namespace OpenGL;


void WindowLayer::Resize(SizeU size) {
	this->size = size;
	invalid_region = invalid_region_front_buffer = RectI(point_i_zero, size);
}

void WindowLayer::Destroy() {}

void WindowLayer::RenderBegin() {
	if (!invalid_region.IsEmpty()) {
		glfwMakeContextCurrent(AsGLFWWindow(window));
	}
}

void WindowLayer::RenderEnd(const Canvas& canvas) {
	RenderContext context(size, nullptr);
	OpenGL::RenderCanvas(static_cast<RenderTarget&>(context), canvas, vector_zero, invalid_region);

	glfwSwapBuffers(AsGLFWWindow(window));
	glfwMakeContextCurrent(nullptr);

	invalid_region = invalid_region_front_buffer;
	invalid_region_front_buffer = rect_i_empty;
}


} // namespace ViewDesign

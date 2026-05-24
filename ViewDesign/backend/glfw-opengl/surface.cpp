#include "ViewDesign/drawing/surface.h"

#include <ViewDesign/platform/glad/render_target.h>
#include <ViewDesign/platform/glfw/window.h>


namespace ViewDesign {

using namespace GLFW;
using namespace OpenGL;


void Surface::Resize(SizeU size) {
	this->size = size;
	invalid_region = invalid_region_front_buffer = invalid_region_back_buffer = RectI(point_i_zero, size);
}

void Surface::Destroy() {}

void Surface::RenderBegin() {
	invalid_region_front_buffer = invalid_region_front_buffer.Union(invalid_region);
	invalid_region = invalid_region_back_buffer = invalid_region_back_buffer.Union(invalid_region);
	if (!invalid_region.IsEmpty()) {
		glfwMakeContextCurrent(AsGLFWWindow(window));
	}
}

void Surface::RenderEnd(const Canvas& canvas) {
	RenderContext context(size, nullptr);
	ViewDesign::RenderCanvas(static_cast<RenderTarget&>(context), canvas, vector_zero, invalid_region);

	glfwSwapBuffers(AsGLFWWindow(window));
	glfwMakeContextCurrent(nullptr);

	invalid_region_back_buffer = invalid_region_front_buffer;
	invalid_region = invalid_region_front_buffer = rect_i_empty;
}


} // namespace ViewDesign

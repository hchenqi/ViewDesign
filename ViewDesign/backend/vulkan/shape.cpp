#include "ViewDesign/drawing/shape.h"
#include "ViewDesign/drawing/helper.h"
#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

using namespace Vulkan;


void Line::DrawOn(RenderTarget& target, Point point) const {}

void Rectangle::DrawOn(RenderTarget& target, Point point) const {
	target.BindFlatPipeline();

	if (fill_color.IsVisible()) {
		target.SetColor(fill_color);

		auto [x0, y0, x1, y1] = AsTuple(Rect(point, size));
		float vertices[] = {
			x0, y0,
			x1, y0,
			x0, y1,
			x1, y1,
			x0, y1,
			x1, y0,
		};
		DeviceContext::Get().CopyVertexBufferMemory(vertices);
		target.bindVertexBuffers(0, *DeviceContext::Get().GetVertexBuffer(), { 0 });
		target.draw(6, 1, 0, 0);
	}
}

void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {}

void Ellipse::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

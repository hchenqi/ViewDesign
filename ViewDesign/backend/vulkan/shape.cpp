#include "ViewDesign/drawing/shape.h"
#include "ViewDesign/platform/vulkan/render_target.h"


namespace ViewDesign {

using namespace Vulkan;


void Line::DrawOn(RenderTarget& target, Point point) const {
	if (begin == end) {
		return Rectangle(Size(width, width), color).DrawOn(target, point + begin - Vector(width, width) / 2.0f);
	}
	target.BindPipeline<FlatPipeline>();
	target.SetColor(color);
	Point begin = point + this->begin, end = point + this->end;
	Vector offset = Normal(Normalize(end - begin)) * (width / 2.0f);
	target.DrawVertices(GetVertices(Quad{ begin - offset, begin + offset, end + offset, end - offset }));
}

void Rectangle::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<FlatPipeline>();
	if (fill_color.IsVisible()) {
		target.SetColor(fill_color);
		target.DrawVertices(GetVertices(AsQuad(Rect(point, size))));
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		target.SetColor(border_color);
		auto outer = AsQuad(Rect(point, size)), inner = AsQuad(Extend(Rect(point, size), -Margin(border_width)));
		std::array quad_list = {
			Quad{ outer[0], outer[1], inner[1], inner[0] },
			Quad{ outer[1], outer[2], inner[2], inner[1] },
			Quad{ outer[2], outer[3], inner[3], inner[2] },
			Quad{ outer[3], outer[0], inner[0], inner[3] },
		};
		target.DrawVertices(GetVertices(quad_list));
	}
}

void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {}

void Ellipse::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

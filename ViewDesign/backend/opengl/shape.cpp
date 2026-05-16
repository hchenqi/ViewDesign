#include "ViewDesign/drawing/shape.h"
#include "ViewDesign/geometry/helper.h"
#include "ViewDesign/platform/glad/render_target.h"


#include <tuple>


namespace ViewDesign {

using namespace OpenGL;


void Line::DrawOn(RenderTarget& target, Point point) const {
	auto [r, g, b, a] = AsTupleNormalizedPremultiplied(color);
	glColor4f(r, g, b, a);

	glLineWidth(width * target.GetCurrentScale());

	glBegin(GL_LINES);
	glVertex2f(point.x + begin.x, point.y + begin.y);
	glVertex2f(point.x + end.x, point.y + end.y);
	glEnd();
}

void Rectangle::DrawOn(RenderTarget& target, Point point) const {
	if (fill_color.IsVisible()) {
		auto [r, g, b, a] = AsTupleNormalizedPremultiplied(fill_color);
		glColor4f(r, g, b, a);

		auto [x0, y0, x1, y1] = AsTuple(Rect(point, size));
		glBegin(GL_POLYGON);
		glVertex2f(x0, y0);
		glVertex2f(x1, y0);
		glVertex2f(x1, y1);
		glVertex2f(x0, y1);
		glEnd();
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		auto [r, g, b, a] = AsTupleNormalizedPremultiplied(border_color);
		glColor4f(r, g, b, a);

		glLineWidth(border_width * target.GetCurrentScale());

		auto [x0, y0, x1, y1] = AsTuple(Extend(Rect(point, size), -Margin(border_width / 2.0f)));
		glBegin(GL_LINE_STRIP);
		glVertex2f(x0, y0);
		glVertex2f(x1, y0);
		glVertex2f(x1, y1);
		glVertex2f(x0, y1);
		glVertex2f(x0, y0);
		glEnd();
	}
}

void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {
	Rectangle(size, fill_color, border_width, border_color).DrawOn(target, point);
}

void Ellipse::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

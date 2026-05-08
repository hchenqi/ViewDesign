#include "ViewDesign/drawing/shape.h"

#include <tuple>

#include <glad/glad.h>


namespace ViewDesign {

namespace {

inline std::tuple<float, float, float, float> AsOpenGLColor(Color color) {
	return { color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f };
}

inline std::tuple<float, float, float, float> AsOpenGLRect(Rect rect) {
	return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

inline std::tuple<float, float, float, float> AsOpenGLRectShrinkBy(Rect rect, float length) {
	return { rect.left() + length, rect.top() + length, rect.right() - length, rect.bottom() - length };
}

} // namespace


void Line::DrawOn(RenderTarget& target, Point point) const {
	auto [r, g, b, a] = AsOpenGLColor(color);
	glColor4f(r, g, b, a);
	glLineWidth(width);
	glBegin(GL_LINES);
	glVertex2f(point.x + begin.x, point.y + begin.y);
	glVertex2f(point.x + end.x, point.y + end.y);
	glEnd();
}

void Rectangle::DrawOn(RenderTarget& target, Point point) const {
	if (fill_color.IsVisible()) {
		{
			auto [r, g, b, a] = AsOpenGLColor(fill_color);
			glColor4f(r, g, b, a);
		}
		{
			auto [l, t, r, b] = AsOpenGLRect(Rect(point, size));
			glBegin(GL_POLYGON);
			glVertex2f(l, t);
			glVertex2f(r, t);
			glVertex2f(r, b);
			glVertex2f(l, b);
			glEnd();
		}
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		{
			auto [r, g, b, a] = AsOpenGLColor(border_color);
			glColor4f(r, g, b, a);
		}
		glLineWidth(border_width);
		{
			auto [l, t, r, b] = AsOpenGLRectShrinkBy(Rect(point, size), border_width / 2.0f);
			glBegin(GL_LINE_STRIP);
			glVertex2f(l, t);
			glVertex2f(r, t);
			glVertex2f(r, b);
			glVertex2f(l, b);
			glVertex2f(l, t);
			glEnd();
		}
	}
}

void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {
	Rectangle(size, fill_color, border_width, border_color).DrawOn(target, point);
}

void Ellipse::DrawOn(RenderTarget& target, Point point) const {}


} // namespace ViewDesign

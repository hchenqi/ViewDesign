#include "ViewDesign/view/figure/shape.h"
#include "ViewDesign/drawing/helper.h"

#include <ViewDesign/platform/glad/render_target.h>


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


namespace {

constexpr size_t kRoundedRectangleCornerSegments = 8;
constexpr size_t kEllipseSegments = 32;
constexpr float kPi = 3.14159265358979323846f;

inline float ClampRadius(float radius, Size size) {
    float max_radius = size.width < size.height ? size.width * 0.5f : size.height * 0.5f;
    return radius < max_radius ? radius : max_radius;
}

inline void DrawRoundedRectanglePath(Point point, Size size, float radius) {
    radius = ClampRadius(radius, size);
    float x0 = point.x;
    float y0 = point.y;
    float x1 = point.x + size.width;
    float y1 = point.y + size.height;

    if (radius <= 0.0f) {
        glVertex2f(x0, y0);
        glVertex2f(x1, y0);
        glVertex2f(x1, y1);
        glVertex2f(x0, y1);
        return;
    }

    auto drawCorner = [&](float cx, float cy, float begin_angle, float end_angle) {
        float angle_step = (end_angle - begin_angle) / static_cast<float>(kRoundedRectangleCornerSegments);
        for (size_t i = 0; i <= kRoundedRectangleCornerSegments; ++i) {
            float angle = begin_angle + angle_step * static_cast<float>(i);
            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
        }
    };

    drawCorner(x0 + radius, y0 + radius, kPi, kPi * 1.5f);
    drawCorner(x1 - radius, y0 + radius, kPi * 1.5f, kPi * 2.0f);
    drawCorner(x1 - radius, y1 - radius, 0.0f, kPi * 0.5f);
    drawCorner(x0 + radius, y1 - radius, kPi * 0.5f, kPi);
}

inline void DrawEllipsePath(Point center, float radius_x, float radius_y) {
    for (size_t i = 0; i < kEllipseSegments; ++i) {
        float angle = kPi * 2.0f * static_cast<float>(i) / static_cast<float>(kEllipseSegments);
        glVertex2f(center.x + cos(angle) * radius_x, center.y + sin(angle) * radius_y);
    }
}

} // namespace


void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {
    if (fill_color.IsVisible()) {
        auto [r, g, b, a] = AsTupleNormalizedPremultiplied(fill_color);
        glColor4f(r, g, b, a);

        glBegin(GL_POLYGON);
        DrawRoundedRectanglePath(point, size, radius);
        glEnd();
    }
    if (border_width > 0.0f && border_color.IsVisible()) {
        auto [r, g, b, a] = AsTupleNormalizedPremultiplied(border_color);
        glColor4f(r, g, b, a);

        glLineWidth(border_width * target.GetCurrentScale());

        Rect border_rect = Extend(Rect(point, size), -Margin(border_width / 2.0f));
        glBegin(GL_LINE_LOOP);
        DrawRoundedRectanglePath(border_rect.point, border_rect.size, radius - border_width / 2.0f);
        glEnd();
    }
}

void Ellipse::DrawOn(RenderTarget& target, Point point) const {
    if (fill_color.IsVisible()) {
        auto [r, g, b, a] = AsTupleNormalizedPremultiplied(fill_color);
        glColor4f(r, g, b, a);

        glBegin(GL_POLYGON);
        DrawEllipsePath(point, radius_x, radius_y);
        glEnd();
    }
    if (border_width > 0.0f && border_color.IsVisible()) {
        auto [r, g, b, a] = AsTupleNormalizedPremultiplied(border_color);
        glColor4f(r, g, b, a);

        glLineWidth(border_width * target.GetCurrentScale());

        glBegin(GL_LINE_LOOP);
        DrawEllipsePath(point, radius_x - border_width / 2.0f, radius_y - border_width / 2.0f);
        glEnd();
    }
}


} // namespace ViewDesign

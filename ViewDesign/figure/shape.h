#pragma once

#include "figure.h"
#include "color.h"


namespace ViewDesign {


struct Line : Figure {
	Vector begin;
	Vector end;
	float width;
	Color color;

	Line(Vector begin, Vector end, float width, Color color) : begin(begin), end(end), width(width), color(color) {}
	Line(Vector end, float width, Color color) : Line(vector_zero, end, width, color) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


struct Rectangle : Figure {
	Size size;
	Color fill_color;
	float border_width;
	Color border_color;

	Rectangle(Size size, Color fill_color, float border_width, Color border_color) : size(size), fill_color(fill_color), border_width(border_width), border_color(border_color) {}
	Rectangle(Size size, Color fill_color) : Rectangle(size, fill_color, 0.0f, color_transparent) {}
	Rectangle(Size size, float border_width, Color border_color) : Rectangle(size, color_transparent, border_width, border_color) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


struct RoundedRectangle : Figure {
	Size size;
	float radius;
	Color fill_color;
	float border_width;
	Color border_color;

	RoundedRectangle(Size size, float radius, Color fill_color, float border_width, Color border_color) : size(size), radius(radius), fill_color(fill_color), border_width(border_width), border_color(border_color) {}
	RoundedRectangle(Size size, float radius, Color fill_color) : RoundedRectangle(size, radius, fill_color, 0.0f, color_transparent) {}
	RoundedRectangle(Size size, float radius, float border_width, Color border_color) : RoundedRectangle(size, radius, color_transparent, border_width, border_color) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


struct Ellipse : Figure {
	float radius_x;
	float radius_y;
	Color fill_color;
	float border_width;
	Color border_color;

	Ellipse(float radius_x, float radius_y, Color fill_color, float border_width, Color border_color) : radius_x(radius_x), radius_y(radius_y), fill_color(fill_color), border_width(border_width), border_color(border_color) {}
	Ellipse(float radius_x, float radius_y, Color fill_color) : Ellipse(radius_x, radius_y, fill_color, 0.0f, color_transparent) {}
	Ellipse(float radius_x, float radius_y, float border_width, Color border_color) : Ellipse(radius_x, radius_y, color_transparent, border_width, border_color) {}

	virtual void DrawOn(RenderTarget& target, Point point) const override;
};


struct Circle : public Ellipse {
	Circle(float radius, Color fill_color, float border_width, Color border_color) : Ellipse(radius, radius, fill_color, border_width, border_color) {}
	Circle(float radius, Color fill_color) : Ellipse(radius, radius, fill_color, 0.0f, color_transparent) {}
	Circle(float radius, float border_width, Color border_color) : Ellipse(radius, radius, color_transparent, border_width, border_color) {}
};


} // namespace ViewDesign

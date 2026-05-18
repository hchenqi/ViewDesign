#pragma once

#include "ViewDesign/drawing/color.h"
#include "ViewDesign/geometry/helper.h"

#include <array>
#include <cmath>
#include <stdexcept>


namespace ViewDesign {


constexpr Color AsPremultiplied(Color color) {
	color.blue = (color.blue * color.alpha + 0x7F) / 0xFF;
	color.green = (color.green * color.alpha + 0x7F) / 0xFF;
	color.red = (color.red * color.alpha + 0x7F) / 0xFF;
	return color;
}

constexpr std::tuple<float, float, float, float> AsTupleNormalizedPremultiplied(Color color) {
	float alpha = color.alpha / 255.0f;
	return { color.red * alpha / 255.0f, color.green * alpha / 255.0f, color.blue * alpha / 255.0f, alpha };
}


constexpr std::tuple<float, float, float, float> AsTuple(Rect rect) {
	return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

constexpr std::tuple<float, float, float, float> AsTupleNormalized(Size size, Rect rect) {
	return { rect.left() / size.width, rect.top() / size.height, rect.right() / size.width, rect.bottom() / size.height };
}


using Quad = std::array<Point, 4>;

constexpr Quad AsQuad(Rect rect) {
	auto [x0, y0, x1, y1] = AsTuple(rect);
	return { Point(x0, y0), Point(x1, y0), Point(x1, y1), Point(x0, y1) };
}

constexpr std::array<Point, 6> GetVertices(Quad quad) {
	return { quad[0], quad[1], quad[2], quad[0], quad[2], quad[3] };
}

template<size_t size>
constexpr std::array<Point, size * 6> GetVertices(std::array<Quad, size> quad_list) {
	std::array<Point, size * 6> vertices;
	for (size_t i = 0; i < size; ++i) {
		vertices[i * 6 + 0] = quad_list[i][0];
		vertices[i * 6 + 1] = quad_list[i][1];
		vertices[i * 6 + 2] = quad_list[i][2];
		vertices[i * 6 + 3] = quad_list[i][0];
		vertices[i * 6 + 4] = quad_list[i][2];
		vertices[i * 6 + 5] = quad_list[i][3];
	}
	return vertices;
}


inline Vector Normalize(Vector vector) {
	if (vector == vector_zero) { throw std::invalid_argument("normalizing zero vector"); }
	float length = sqrt(square(vector.x) + square(vector.y));
	return Vector(vector.x / length, vector.y / length);
}

constexpr Vector Normal(Vector vector) {
	return Vector(-vector.y, vector.x);
}


} // namespace ViewDesign

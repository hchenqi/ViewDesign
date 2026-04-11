#pragma once

#include "../common/core.h"


namespace ViewDesign {


struct Vector {
	float x;
	float y;

	explicit constexpr Vector() : x(), y() {}
	explicit constexpr Vector(float x, float y) : x(x), y(y) {}

	constexpr bool operator==(const Vector& vector) const { return x == vector.x && y == vector.y; }
	constexpr bool operator!=(const Vector& vector) const { return x != vector.x || y != vector.y; }

	constexpr Vector operator-() const { return Vector(-x, -y); }

	constexpr Vector operator+(const Vector& vector) const { return Vector(x + vector.x, y + vector.y); }
	constexpr Vector operator-(const Vector& vector) const { return Vector(x - vector.x, y - vector.y); }
	constexpr Vector& operator+=(const Vector& vector) { return (*this) = (*this) + vector; }
	constexpr Vector& operator-=(const Vector& vector) { return (*this) = (*this) - vector; }
};


} // namespace ViewDesign

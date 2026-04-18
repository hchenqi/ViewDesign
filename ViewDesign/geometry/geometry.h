#pragma once

#include "point.h"
#include "vector.h"
#include "size.h"
#include "rect.h"
#include "scale.h"


namespace ViewDesign {


constexpr Vector vector_zero = Vector(0.0f, 0.0f);

constexpr float length_max = 1e8f;
constexpr float length_min = 0.0f;
constexpr Size size_max = Size(length_max, length_max);
constexpr Size size_min = Size(length_min, length_min);
constexpr Size size_empty = size_min;

constexpr float position_max = length_max / 2;
constexpr float position_min = -position_max;
constexpr Point point_max = Point(position_max, position_max);
constexpr Point point_min = Point(position_min, position_min);
constexpr Point point_zero = Point(0.0f, 0.0f);

constexpr Rect region_empty = Rect(point_zero, size_empty);
constexpr Rect region_infinite = Rect(point_min, size_max);


constexpr Point operator-(Point point) { return Point(-point.x, -point.y); }
constexpr Vector operator-(Point end, Point begin) { return Vector(end.x - begin.x, end.y - begin.y); }

constexpr Point operator+(Point point, Vector vector) { return Point(point.x + vector.x, point.y + vector.y); }
constexpr Point operator-(Point point, Vector vector) { return Point(point.x - vector.x, point.y - vector.y); }
constexpr Point& operator+=(Point& point, Vector vector) { return point = point + vector; }
constexpr Point& operator-=(Point& point, Vector vector) { return point = point - vector; }

constexpr Rect operator+(Rect rect, Vector vector) { return Rect(rect.point + vector, rect.size); }
constexpr Rect operator-(Rect rect, Vector vector) { return Rect(rect.point - vector, rect.size); }
constexpr Rect& operator+=(Rect& rect, Vector vector) { return rect = rect + vector; }
constexpr Rect& operator-=(Rect& rect, Vector vector) { return rect = rect - vector; }


constexpr Vector operator*(Vector vector, Scale scale) { return Vector(vector.x * scale.x, vector.y * scale.y); }
constexpr Size operator*(Size size, Scale scale) { return Size(size.width * scale.x, size.height * scale.y); }
constexpr Point operator*(Point point, Scale scale) { return Point(point.x * scale.x, point.y * scale.y); }
constexpr Rect operator*(Rect rect, Scale scale) { return Rect(rect.point * scale, rect.size * scale); }

constexpr Vector& operator*=(Vector& vector, Scale scale) { return vector = vector * scale; }
constexpr Size& operator*=(Size& size, Scale scale) { return size = size * scale; }
constexpr Point& operator*=(Point& point, Scale scale) { return point = point * scale; }
constexpr Rect& operator*=(Rect& rect, Scale scale) { return rect = rect * scale; }


} // namespace ViewDesign

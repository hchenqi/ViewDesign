#pragma once

#include "ViewDesign/geometry/rect.h"
#include "ViewDesign/geometry/vector.h"
#include "ViewDesign/geometry/scale.h"


namespace ViewDesign {


constexpr float length_zero = 0.0f;
constexpr float length_infinite = 1e8f;

constexpr float position_max = length_infinite / 2;
constexpr float position_min = -position_max;
constexpr float position_zero = 0.0f;

constexpr Point point_max = Point(position_max, position_max);
constexpr Point point_min = Point(position_min, position_min);
constexpr Point point_zero = Point(position_zero, position_zero);

constexpr Size size_empty = Size(length_zero, length_zero);
constexpr Size size_infinite = Size(length_infinite, length_infinite);

constexpr Interval interval_empty = Interval(position_zero, length_zero);
constexpr Interval interval_infinite = Interval(position_min, length_infinite);

constexpr Rect rect_empty = Rect(point_zero, size_empty);
constexpr Rect rect_infinite = Rect(point_min, size_infinite);

constexpr Vector operator-(Point end, Point begin) { return Vector(end.x - begin.x, end.y - begin.y); }

constexpr Vector vector_zero = point_zero - point_zero;

constexpr Point operator+(Point point, Vector vector) { return Point(point.x + vector.x, point.y + vector.y); }
constexpr Point& operator+=(Point& point, Vector vector) { return point = point + vector; }
constexpr Point operator-(Point point, Vector vector) { return Point(point.x - vector.x, point.y - vector.y); }
constexpr Point& operator-=(Point& point, Vector vector) { return point = point - vector; }

constexpr Rect operator+(Rect rect, Vector vector) { return Rect(rect.point + vector, rect.size); }
constexpr Rect& operator+=(Rect& rect, Vector vector) { return rect = rect + vector; }
constexpr Rect operator-(Rect rect, Vector vector) { return Rect(rect.point - vector, rect.size); }
constexpr Rect& operator-=(Rect& rect, Vector vector) { return rect = rect - vector; }

constexpr Scale scale_identity = Scale(1.0f);

constexpr Point operator*(Point point, Scale scale) { return Point(point.x * scale.x, point.y * scale.y); }
constexpr Point& operator*=(Point& point, Scale scale) { return point = point * scale; }
constexpr Point operator/(Point point, Scale scale) { return Point(point.x / scale.x, point.y / scale.y); }
constexpr Point& operator/=(Point& point, Scale scale) { return point = point / scale; }

constexpr Size operator*(Size size, Scale scale) { return Size(size.width * scale.x, size.height * scale.y); }
constexpr Size& operator*=(Size& size, Scale scale) { return size = size * scale; }
constexpr Size operator/(Size size, Scale scale) { return Size(size.width / scale.x, size.height / scale.y); }
constexpr Size& operator/=(Size& size, Scale scale) { return size = size / scale; }

constexpr Rect operator*(Rect rect, Scale scale) { return Rect(rect.point * scale, rect.size * scale); }
constexpr Rect& operator*=(Rect& rect, Scale scale) { return rect = rect * scale; }
constexpr Rect operator/(Rect rect, Scale scale) { return Rect(rect.point / scale, rect.size / scale); }
constexpr Rect& operator/=(Rect& rect, Scale scale) { return rect = rect / scale; }

constexpr Vector operator*(Vector vector, Scale scale) { return Vector(vector.x * scale.x, vector.y * scale.y); }
constexpr Vector& operator*=(Vector& vector, Scale scale) { return vector = vector * scale; }
constexpr Vector operator/(Vector vector, Scale scale) { return Vector(vector.x / scale.x, vector.y / scale.y); }
constexpr Vector& operator/=(Vector& vector, Scale scale) { return vector = vector / scale; }


} // namespace ViewDesign

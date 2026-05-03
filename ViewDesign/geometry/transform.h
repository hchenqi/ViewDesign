#pragma once

#include "ViewDesign/geometry/geometry.h"


namespace ViewDesign {


struct Transform {
	float matrix[3][2];

	Transform();
	Transform(Vector vector) : Transform(Translation(vector)) {}
	Transform(ViewDesign::Scale scale) : Transform(Scale(scale)) {}

	static Transform Identity() { return Transform(); }
	static Transform Translation(Vector offset);
	static Transform Scale(ViewDesign::Scale scale, Point center);
	static Transform Scale(ViewDesign::Scale scale) { return Scale(scale, point_zero); }
	static Transform Rotation(float angle, Point center);
	static Transform Rotation(float angle) { return Rotation(angle, point_zero); }
	static Transform Skew(float angle_x, float angle_y, Point center);
	static Transform Skew(float angle_x, float angle_y) { return Skew(angle_x, angle_y, point_zero); }

	Transform Invert() const;

	ViewDesign::Scale GetScale() const;
};


Transform operator*(const Transform& applied, const Transform& original);

Point operator*(Point point, const Transform& transform);

inline Point& operator*=(Point& point, const Transform& transform) { return point = point * transform; }


} // namespace ViewDesign

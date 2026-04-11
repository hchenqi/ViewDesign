#pragma once

#include "vector.h"
#include "point.h"
#include "scale.h"


namespace ViewDesign {


struct Transform {
	float matrix[3][2];

	static Transform Identity();
	static Transform Translation(Vector offset);
	static Transform Scale(ViewDesign::Scale scale, Point center);
	static Transform Scale(ViewDesign::Scale scale) { return Scale(scale, Point()); }
	static Transform Rotation(float angle, Point center);
	static Transform Rotation(float angle) { return Rotation(angle, Point()); }
	static Transform Skew(float angle_x, float angle_y, Point center);
	static Transform Skew(float angle_x, float angle_y) { return Skew(angle_x, angle_y, Point()); }

	Transform() : Transform(Identity()) {}
	Transform(Vector vector) : Transform(Translation(vector)) {}
	Transform(ViewDesign::Scale scale) : Transform(Scale(scale)) {}

	Transform Invert() const;

	ViewDesign::Scale GetScale() const;
	bool IsAxisAligned() const;
};


Transform operator*(const Transform& applied, const Transform& original);

Point operator*(Point point, const Transform& transform);

inline Point& operator*=(Point& point, const Transform& transform) { return point = point * transform; }


} // namespace ViewDesign

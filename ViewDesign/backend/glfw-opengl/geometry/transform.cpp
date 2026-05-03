#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/geometry/helper.h"

#include <stdexcept>


namespace ViewDesign {


Transform::Transform() {
	matrix[0][0] = 1.0f; matrix[0][1] = 0.0f;
	matrix[1][0] = 0.0f; matrix[1][1] = 1.0f;
	matrix[2][0] = 0.0f; matrix[2][1] = 0.0f;
}

Transform Transform::Translation(Vector offset) {
	Transform t = Identity();
	t.matrix[2][0] = offset.x;
	t.matrix[2][1] = offset.y;
	return t;
}

Transform Transform::Scale(ViewDesign::Scale scale, Point center) {
	Transform t = Identity();
	t.matrix[0][0] = scale.x;
	t.matrix[1][1] = scale.y;
	t.matrix[2][0] = center.x * (1 - scale.x);
	t.matrix[2][1] = center.y * (1 - scale.y);
	return t;
}

Transform Transform::Rotation(float angle, Point center) {
	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	Transform t = Identity();
	t.matrix[0][0] = cos_a; t.matrix[0][1] = sin_a;
	t.matrix[1][0] = -sin_a; t.matrix[1][1] = cos_a;
	t.matrix[2][0] = center.x * (1 - cos_a) + center.y * sin_a;
	t.matrix[2][1] = center.y * (1 - cos_a) - center.x * sin_a;
	return t;
}

Transform Transform::Skew(float angle_x, float angle_y, Point center) {
	float tan_x = tanf(angle_x);
	float tan_y = tanf(angle_y);
	Transform t = Identity();
	t.matrix[0][1] = tan_x;
	t.matrix[1][0] = tan_y;
	t.matrix[2][0] = -center.y * tan_y;
	t.matrix[2][1] = -center.x * tan_x;
	return t;
}

Transform Transform::Invert() const {
	Transform inv;
	float det = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
	if (det == 0) { throw std::runtime_error("transform matrix not invertible"); }
	inv.matrix[0][0] = matrix[1][1] / det;
	inv.matrix[0][1] = -matrix[0][1] / det;
	inv.matrix[1][0] = -matrix[1][0] / det;
	inv.matrix[1][1] = matrix[0][0] / det;
	inv.matrix[2][0] = (-matrix[2][0] * inv.matrix[0][0] - matrix[2][1] * inv.matrix[1][0]);
	inv.matrix[2][1] = (-matrix[2][0] * inv.matrix[0][1] - matrix[2][1] * inv.matrix[1][1]);
	return inv;
}

ViewDesign::Scale Transform::GetScale() const {
	return ViewDesign::Scale(sqrtf(square(matrix[0][0]) + square(matrix[0][1])), sqrtf(square(matrix[1][0]) + square(matrix[1][1])));
}


Transform operator*(const Transform& applied, const Transform& original) {
	Transform result;
	result.matrix[0][0] = applied.matrix[0][0] * original.matrix[0][0] + applied.matrix[0][1] * original.matrix[1][0];
	result.matrix[0][1] = applied.matrix[0][0] * original.matrix[0][1] + applied.matrix[0][1] * original.matrix[1][1];
	result.matrix[1][0] = applied.matrix[1][0] * original.matrix[0][0] + applied.matrix[1][1] * original.matrix[1][0];
	result.matrix[1][1] = applied.matrix[1][0] * original.matrix[0][1] + applied.matrix[1][1] * original.matrix[1][1];
	result.matrix[2][0] = applied.matrix[2][0] * original.matrix[0][0] + applied.matrix[2][1] * original.matrix[1][0] + original.matrix[2][0];
	result.matrix[2][1] = applied.matrix[2][0] * original.matrix[0][1] + applied.matrix[2][1] * original.matrix[1][1] + original.matrix[2][1];
	return result;
}

Point operator*(Point point, const Transform& transform) {
	float x = point.x * transform.matrix[0][0] + point.y * transform.matrix[1][0] + transform.matrix[2][0];
	float y = point.x * transform.matrix[0][1] + point.y * transform.matrix[1][1] + transform.matrix[2][1];
	return Point(x, y);
}


} // namespace ViewDesign

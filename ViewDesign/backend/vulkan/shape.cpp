#include "ViewDesign/view/figure/shape.h"

#include <ViewDesign/platform/vulkan/pipeline/flat.h>


namespace ViewDesign {

using namespace Vulkan;


void Line::DrawOn(RenderTarget& target, Point point) const {
	if (begin == end) {
		return Rectangle(Size(width, width), color).DrawOn(target, point + begin - Vector(width, width) / Scale(2.0f));
	}
	target.BindPipeline<FlatPipeline>();
	FlatPipeline::SetColor(target, color);
	Point begin = point + this->begin, end = point + this->end;
	Vector offset = Normal(Normalize(end - begin)) * Scale(width / 2.0f);
	target.DrawVertices(GetVertices(Quad{ begin - offset, begin + offset, end + offset, end - offset }));
}

void Rectangle::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<FlatPipeline>();
	if (fill_color.IsVisible()) {
		FlatPipeline::SetColor(target, fill_color);
		target.DrawVertices(GetVertices(AsQuad(Rect(point, size))));
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		FlatPipeline::SetColor(target, border_color);
		auto outer = AsQuad(Rect(point, size)), inner = AsQuad(Extend(Rect(point, size), -Margin(border_width)));
		std::array quad_list = {
			Quad{ outer[0], outer[1], inner[1], inner[0] },
			Quad{ outer[1], outer[2], inner[2], inner[1] },
			Quad{ outer[2], outer[3], inner[3], inner[2] },
			Quad{ outer[3], outer[0], inner[0], inner[3] },
		};
		target.DrawVertices(GetVertices(quad_list));
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

inline std::vector<Point> GetRoundedRectanglePath(Point point, Size size, float radius) {
	radius = ClampRadius(radius, size);
	float x0 = point.x;
	float y0 = point.y;
	float x1 = point.x + size.width;
	float y1 = point.y + size.height;

	std::vector<Point> result;
	result.reserve(4 * (kRoundedRectangleCornerSegments + 1));

	auto appendCorner = [&](float cx, float cy, float begin_angle, float end_angle) {
		float angle_step = (end_angle - begin_angle) / static_cast<float>(kRoundedRectangleCornerSegments);
		for (size_t i = 0; i <= kRoundedRectangleCornerSegments; ++i) {
			float angle = begin_angle + angle_step * static_cast<float>(i);
			result.emplace_back(cx + cos(angle) * radius, cy + sin(angle) * radius);
		}
	};

	if (radius <= 0.0f) {
		result.emplace_back(x0, y0);
		result.emplace_back(x1, y0);
		result.emplace_back(x1, y1);
		result.emplace_back(x0, y1);
		return result;
	}

	appendCorner(x0 + radius, y0 + radius, kPi, kPi * 1.5f);
	appendCorner(x1 - radius, y0 + radius, kPi * 1.5f, kPi * 2.0f);
	appendCorner(x1 - radius, y1 - radius, 0.0f, kPi * 0.5f);
	appendCorner(x0 + radius, y1 - radius, kPi * 0.5f, kPi);
	return result;
}

inline std::vector<Point> GetEllipsePath(Point center, float radius_x, float radius_y) {
	std::vector<Point> result;
	result.reserve(kEllipseSegments);
	for (size_t i = 0; i < kEllipseSegments; ++i) {
		float angle = kPi * 2.0f * static_cast<float>(i) / static_cast<float>(kEllipseSegments);
		result.emplace_back(center.x + cos(angle) * radius_x, center.y + sin(angle) * radius_y);
	}
	return result;
}

inline void DrawFilledPath(RenderTarget& target, const std::vector<Point>& path, Point center) {
	std::vector<Point> vertices;
	vertices.reserve(path.size() * 3);
	for (size_t i = 0; i < path.size(); ++i) {
		size_t next = (i + 1) % path.size();
		vertices.push_back(center);
		vertices.push_back(path[i]);
		vertices.push_back(path[next]);
	}
	target.DrawVertices(vertices);
}

inline void DrawBorderPath(RenderTarget& target, const std::vector<Point>& outer, const std::vector<Point>& inner) {
	std::vector<Point> vertices;
	vertices.reserve(outer.size() * 6);
	for (size_t i = 0; i < outer.size(); ++i) {
		size_t next = (i + 1) % outer.size();
		vertices.push_back(outer[i]);
		vertices.push_back(outer[next]);
		vertices.push_back(inner[next]);
		vertices.push_back(outer[i]);
		vertices.push_back(inner[next]);
		vertices.push_back(inner[i]);
	}
	target.DrawVertices(vertices);
}

} // namespace


void RoundedRectangle::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<FlatPipeline>();
	if (fill_color.IsVisible()) {
		FlatPipeline::SetColor(target, fill_color);
		auto path = GetRoundedRectanglePath(point, size, radius);
		Point center = point + Vector(size.width / 2.0f, size.height / 2.0f);
		DrawFilledPath(target, path, center);
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		FlatPipeline::SetColor(target, border_color);
		Size inner_size(
			std::max(0.0f, size.width - border_width * 2.0f),
			std::max(0.0f, size.height - border_width * 2.0f)
		);
		Point inner_point(point.x + border_width, point.y + border_width);
		auto outer = GetRoundedRectanglePath(point, size, radius);
		auto inner = GetRoundedRectanglePath(inner_point, inner_size, std::max(0.0f, radius - border_width));
		if (outer.size() == inner.size()) {
			DrawBorderPath(target, outer, inner);
		}
	}
}

void Ellipse::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<FlatPipeline>();
	if (fill_color.IsVisible()) {
		FlatPipeline::SetColor(target, fill_color);
		auto path = GetEllipsePath(point, radius_x, radius_y);
		DrawFilledPath(target, path, point);
	}
	if (border_width > 0.0f && border_color.IsVisible()) {
		FlatPipeline::SetColor(target, border_color);
		auto outer = GetEllipsePath(point, radius_x, radius_y);
		auto inner = GetEllipsePath(point, std::max(0.0f, radius_x - border_width), std::max(0.0f, radius_y - border_width));
		DrawBorderPath(target, outer, inner);
	}
}


} // namespace ViewDesign

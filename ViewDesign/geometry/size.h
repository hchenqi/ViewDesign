#pragma once

#include <algorithm>


namespace ViewDesign {


struct Size {
	float width;
	float height;

	explicit constexpr Size() = default;
	explicit constexpr Size(float width, float height) : width(width), height(height) {}

	constexpr bool IsEmpty() const { return width == 0.0f || height == 0.0f; }

	constexpr bool operator==(const Size& other) const { return width == other.width && height == other.height; }

	constexpr Size Union(const Size& other) const { return Size(std::max(width, other.width), std::max(height, other.height)); }
	constexpr Size Intersect(const Size& other) const { return Size(std::min(width, other.width), std::min(height, other.height)); }
};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/sizeu.h"
#include "ViewDesign/drawing/color.h"

#include <vector>
#include <algorithm>
#include <stdexcept>


namespace ViewDesign {


class PixelBuffer {
public:
	explicit PixelBuffer(SizeU size) : size(size), pixels(size.width * size.height) {}
	explicit PixelBuffer() : PixelBuffer(size_u_empty) {}
private:
	SizeU size;
	std::vector<Color> pixels;
public:
	SizeU Size() const { return size; }
	const std::vector<Color>& Pixels() const { return pixels; }
	size_t PixelDataLength() const { static_assert(sizeof(Color) == 4); return pixels.size() * 4; }
	const Color* PixelData() const { return pixels.data(); }
public:
	std::vector<Color>& Pixels() { return pixels; }
	PixelBuffer& Clear(Color color) { std::fill(pixels.begin(), pixels.end(), color); return *this; }
	PixelBuffer& Resize(SizeU size) { this->size = size; pixels.resize(size.width * size.height); return *this; }
	PixelBuffer& Resize(SizeU size, Color color) { this->size = size; pixels.resize(size.width * size.height, color); return *this; }
};


} // namespace ViewDesign

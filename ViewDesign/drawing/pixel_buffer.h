#pragma once

#include "ViewDesign/drawing/color.h"

#include <vector>
#include <stdexcept>


namespace ViewDesign {


class PixelBuffer {
public:
	PixelBuffer(uint width, uint height) : width(width), height(height), pixels(width* height) {}
private:
	uint width, height;
	std::vector<Color> pixels;
public:
	std::pair<uint, uint> Size() const { return { width, height }; }
	const std::vector<Color>& Pixels() const { return pixels; }
	std::vector<Color>& Pixels() { return pixels; }
	const Color& At(uint x, uint y) const { if (x >= width || y >= height) { throw std::out_of_range("PixelBuffer::At: out of range"); } return pixels[y * width + x]; }
	Color& At(uint x, uint y) { return const_cast<Color&>(const_cast<const PixelBuffer&>(*this).At(x, y)); }
public:
	void Resize(uint width, uint height) { this->width = width; this->height = height; pixels.resize(width * height); }
};


} // namespace ViewDesign

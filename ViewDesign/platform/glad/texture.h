#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"

#include <glad/glad.h>


namespace ViewDesign {

namespace OpenGL {


class Texture {
public:
	Texture() : id(0) {}
	Texture(const PixelBuffer& pixel_buffer) : id(CreateTexture(pixel_buffer)) {}
	Texture(SizeU size) : id(CreateTexture(size)) {}
	~Texture() { if (id != 0) { glDeleteTextures(1, &id); id = 0; } }

private:
	GLuint id;
public:
	GLuint GetId() const { return id; }

private:
	inline static PixelBuffer ConvertPixelBuffer(PixelBuffer pixel_buffer) {
		for (auto& color : pixel_buffer.Pixels()) {
			uint8 alpha = color.alpha;
			color.blue = (color.blue * alpha + 0x7F) / 0xFF;
			color.green = (color.green * alpha + 0x7F) / 0xFF;
			color.red = (color.red * alpha + 0x7F) / 0xFF;
		}
		return pixel_buffer;
	}
private:
	inline static GLuint CreateTexture(const PixelBuffer& pixel_buffer) {
		auto [width, height] = pixel_buffer.Size();
		GLuint texture = 0;
		glGenTextures(1, &texture);
		if (texture == 0) {
			throw std::runtime_error("OpenGL: texture creation failed");
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, ConvertPixelBuffer(pixel_buffer).Pixels().data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}
	inline static GLuint CreateTexture(SizeU size) {
		GLuint texture = 0;
		glGenTextures(1, &texture);
		if (texture == 0) {
			throw std::runtime_error("OpenGL: texture creation failed");
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.width, size.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}
};


} // namespace OpenGL

} // namespace ViewDesign

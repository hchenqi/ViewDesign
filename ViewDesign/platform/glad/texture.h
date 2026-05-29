#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/drawing/helper.h"

#include <glad/glad.h>


namespace ViewDesign {

namespace OpenGL {


class Texture {
public:
	Texture() : Texture(0) {}
	Texture(GLuint id) : id(id) {}
	Texture(const PixelBuffer& pixel_buffer) : Texture(CreateTexture(pixel_buffer)) {}
	Texture(SizeU size) : Texture(CreateTexture(size)) {}
public:
	~Texture() { if (id != 0) { glDeleteTextures(1, &id); id = 0; } }

private:
	GLuint id;
public:
	GLuint GetId() const { return id; }

private:
	static PixelBuffer ConvertPixelBuffer(PixelBuffer pixel_buffer) {
		for (auto& color : pixel_buffer.Pixels()) {
			color = AsPremultiplied(color);
		}
		return pixel_buffer;
	}
private:
	static GLuint CreateTexture(const PixelBuffer& pixel_buffer) {
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
	static GLuint CreateTexture(SizeU size) {
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

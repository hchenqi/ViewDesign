#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/geometry/size.h"

#include <glad/glad.h>


namespace ViewDesign {

namespace OpenGL {


class Texture {
public:
	Texture() : id(0) {}
	Texture(const PixelBuffer& pixel_buffer) : id(CreateTexture(pixel_buffer)) {}
	Texture(Size size) : id(CreateTexture(size)) {}
	~Texture() { if (id != 0) { glDeleteTextures(1, &id); id = 0; } }

private:
	GLuint id;
public:
	GLuint GetId() const { return id; }

private:
	inline static GLuint CreateTexture(const PixelBuffer& pixel_buffer) {
		auto [width, height] = pixel_buffer.Size();
		GLuint texture = 0;
		glGenTextures(1, &texture);
		if (texture == 0) {
			throw std::runtime_error("OpenGL: texture creation failed");
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixel_buffer.Pixels().data());
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}
	inline static GLuint CreateTexture(Size size) {
		auto [width, height] = std::make_pair((uint)ceilf(size.width), (uint)ceilf(size.height));
		GLuint texture = 0;
		glGenTextures(1, &texture);
		if (texture == 0) {
			throw std::runtime_error("OpenGL: texture creation failed");
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
		return texture;
	}
};


} // namespace OpenGL

} // namespace ViewDesign

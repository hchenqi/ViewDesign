#pragma once

#include "ViewDesign/drawing/pixel_buffer.h"
#include "ViewDesign/geometry/geometry.h"

#include <glad/glad.h>


namespace ViewDesign {

namespace OpenGL {

inline GLuint CreateTexture(const PixelBuffer& pixel_buffer) {
	auto [width, height] = pixel_buffer.Size();
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixel_buffer.Pixels().data());
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

inline GLuint CreateTexture(Size size) {
	auto [width, height] = std::make_pair((uint)ceilf(size.width), (uint)ceilf(size.height));
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

inline GLuint CreateFrameBuffer(GLuint texture) {
	GLuint frame_buffer;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// something is wrong
	}
	return frame_buffer;
}

} // namespace OpenGL


struct TextureResource {
	struct Texture {
		GLuint id;
		Texture(GLuint id) : id(id) {}
		~Texture() { if (id != 0) { glDeleteTextures(1, &id); } }
	} texture;

	struct FrameBuffer {
		GLuint id;
		FrameBuffer(GLuint id) : id(id) {}
		~FrameBuffer() { if (id != 0) { glDeleteFramebuffers(1, &id); } }
	} frame_buffer;

	TextureResource(const PixelBuffer& pixel_buffer) : texture(OpenGL::CreateTexture(pixel_buffer)), frame_buffer(0) {}
	TextureResource(Size size) : texture(OpenGL::CreateTexture(size)), frame_buffer(OpenGL::CreateFrameBuffer(texture.id)) {}
	TextureResource() : texture(0), frame_buffer(0) {}
};


} // namespace ViewDesign

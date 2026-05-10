#pragma once

#include "ViewDesign/platform/glad/texture.h"


namespace ViewDesign {

namespace OpenGL {


class FrameBuffer : public Texture {
public:
	FrameBuffer() : Texture(), id(0) {}
	FrameBuffer(Size size) : Texture(size), id(CreateFrameBuffer(Texture::GetId())) {}
	~FrameBuffer() { if (id != 0) { glDeleteFramebuffers(1, &id); id = 0; } }

private:
	GLuint id;
public:
	GLuint GetId() const { return id; }

private:
	inline static GLuint CreateFrameBuffer(GLuint texture) {
		GLuint frame_buffer;
		glGenFramebuffers(1, &frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("OpenGL: framebuffer creation failed");
		}
		return frame_buffer;
	}
};


} // namespace OpenGL

} // namespace ViewDesign

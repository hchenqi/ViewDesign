#pragma once

#include "ViewDesign/platform/glad/texture.h"


namespace ViewDesign {

namespace OpenGL {


class Framebuffer : public Texture {
public:
	Framebuffer() : Texture(), id(0) {}
	Framebuffer(SizeU size) : Texture(size), id(Create(Texture::GetId())) {}
	~Framebuffer() { if (id != 0) { glDeleteFramebuffers(1, &id); id = 0; } }

private:
	GLuint id;
public:
	GLuint GetId() const { return id; }

private:
	static GLuint Create(GLuint texture) {
		GLuint framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("OpenGL: framebuffer creation failed");
		}
		return framebuffer;
	}
};


} // namespace OpenGL

} // namespace ViewDesign

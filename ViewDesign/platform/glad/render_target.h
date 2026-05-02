#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/platform/glad/texture.h"

#include <vector>


namespace ViewDesign {


class RenderTarget {
private:
	Size size;
	std::vector<Rect> clip_stack;
public:
	RenderTarget(const TextureResource& texture, Size size) : size(size) {
		glBindFramebuffer(GL_FRAMEBUFFER, texture.frame_buffer.id);

		auto [width, height] = std::make_pair((uint)ceilf(size.width), (uint)ceilf(size.height));
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_SCISSOR_TEST);
	}
public:
	void SetTransform(const Transform& transform) {
		const auto& mat32 = transform.matrix;
		float mat44[16] = {
			mat32[0][0], mat32[0][1], 0.0f, 0.0f,
			mat32[1][0], mat32[1][1], 0.0f, 0.0f,
			0.0f,        0.0f,        1.0f, 0.0f,
			mat32[2][0], mat32[2][1], 0.0f, 1.0f,
		};
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(mat44);
	}
private:
	void SetAxisAlignedClip(const Rect& rect) {
		glScissor(rect.point.x, size.height - rect.point.y - rect.size.height, rect.size.width, rect.size.height);
	}
public:
	void PushAxisAlignedClip(Rect rect) {
		SetAxisAlignedClip(rect);
		clip_stack.push_back(rect);
	}
	void PopAxisAlignedClip() {
		clip_stack.pop_back();
		SetAxisAlignedClip(clip_stack.back());
	}
public:
	void Clear() {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
};


} // namespace ViewDesign

#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/platform/glad/framebuffer.h"

#include <vector>


namespace ViewDesign {

namespace OpenGL {


struct RenderContext {
public:
	RenderContext(SizeU size, ref_ptr<Framebuffer> framebuffer) : size(size) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer == nullptr ? 0 : framebuffer->GetId());

		glViewport(0, 0, size.width, size.height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, size.width, size.height, 0, -1, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_SCISSOR_TEST);

		clip_stack.emplace_back(point_i_zero, size);
		SetClip(clip_stack.back());
	}

	// context
private:
	SizeU size;
	Scale current_scale;
	Transform current_transform;
public:
	void SetTransform(const Transform& transform) {
		current_scale = transform.GetScale();
		current_transform = transform;
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
public:
	float GetCurrentScale() const {
		return (current_scale.x + current_scale.y) / 2;
	}

	// clip
private:
	std::vector<RectI> clip_stack;
private:
	void SetClip(RectI rect) {
		glScissor(rect.point.x, size.height - rect.point.y - rect.size.height, rect.size.width, rect.size.height);
	}
public:
	void PushClip(Rect clip_region) {
		RectI rect = clip_stack.back().Intersect(Round(GetBoundingRectAfterTransform(clip_region, current_transform)));
		if (rect != clip_stack.back()) {
			SetClip(rect);
		}
		clip_stack.emplace_back(rect);
	}
	void PopClip() {
		RectI rect = clip_stack.back();
		clip_stack.pop_back();
		if (clip_stack.back() != rect) {
			SetClip(clip_stack.back());
		}
	}

public:
	void Clear() {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
};


} // namespace OpenGL


struct RenderTarget : OpenGL::RenderContext {};


} // namespace ViewDesign

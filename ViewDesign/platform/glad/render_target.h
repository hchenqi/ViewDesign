#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/geometry/helper.h"
#include "ViewDesign/platform/glad/framebuffer.h"

#include <vector>


namespace ViewDesign {

namespace OpenGL {


struct RenderContext {
private:
	SizeU size;
	std::vector<RectI> clip_stack;
	Scale current_scale;
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
	}
public:
	void SetTransform(const Transform& transform) {
		current_scale = transform.GetScale();
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
	float GetCurrentScale() const {
		return (current_scale.x + current_scale.y) / 2;
	}
private:
	void SetAxisAlignedClip(RectI rect) {
		glScissor(rect.point.x, size.height - rect.point.y - rect.size.height, rect.size.width, rect.size.height);
	}
public:
	void PushAxisAlignedClip(Rect rect) {
		RectI rect_i = Round(rect);
		SetAxisAlignedClip(rect_i);
		clip_stack.push_back(rect_i);
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


inline std::tuple<float, float, float, float> AsOpenGLColor(Color color) {
	float alpha = color.alpha / 255.0f;
	return { color.red * alpha / 255.0f, color.green * alpha / 255.0f, color.blue * alpha / 255.0f, alpha };
}

inline std::tuple<float, float, float, float> AsOpenGLRect(Rect rect) {
	return { rect.left(), rect.top(), rect.right(), rect.bottom() };
}

inline std::tuple<float, float, float, float> AsOpenGLRectRatio(Size size, Rect rect) {
	return { rect.left() / size.width, rect.top() / size.height, rect.right() / size.width, rect.bottom() / size.height };
}

inline std::tuple<float, float, float, float> AsOpenGLRectShrinkBy(Rect rect, float length) {
	return { rect.left() + length, rect.top() + length, rect.right() - length, rect.bottom() - length };
}


} // namespace OpenGL


struct RenderTarget : OpenGL::RenderContext {};


} // namespace ViewDesign

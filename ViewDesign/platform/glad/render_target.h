#pragma once

#include "ViewDesign/geometry/transform.h"
#include "ViewDesign/platform/glad/framebuffer.h"

#include <vector>


namespace ViewDesign {

namespace OpenGL {


struct RenderContext {
private:
	Size size;
	std::vector<Rect> clip_stack;
public:
	RenderContext(Size size, ref_ptr<Framebuffer> framebuffer) : size(size) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer == nullptr ? 0 : framebuffer->GetId());

		auto [width, height] = std::make_pair((uint)ceilf(size.width), (uint)ceilf(size.height));
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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


inline struct RenderTarget {} render_target;


} // namespace ViewDesign

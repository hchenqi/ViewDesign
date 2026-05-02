#include "ViewDesign/system/cursor.h"
#include "ViewDesign/platform/glfw/cursor.h"
#include "ViewDesign/platform/glfw/window.h"
#include "ViewDesign/platform/glfw/context.h"

#include <array>
#include <vector>


namespace ViewDesign {

using namespace GLFW;


class Cursor {
public:
	enum class Type {
		Hidden,
		NoChange,
		Shared,
		Custom,
	};
public:
	Cursor(Type type, GLFWcursor* handle) : type(type), handle(handle) {}
	~Cursor() { if (Context::Initialized() && handle != nullptr) { glfwDestroyCursor(handle); } }
public:
	Type type;
	GLFWcursor* handle;
};


namespace {

std::array<Cursor, static_cast<size_t>(CursorStyle::_Count)> cursor_cache_shared = {
	Cursor(Cursor::Type::Hidden, nullptr),
	Cursor(Cursor::Type::NoChange, nullptr),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_ARROW_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_IBEAM_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR)),
	Cursor(Cursor::Type::Shared, nullptr),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR)),
	Cursor(Cursor::Type::Shared, nullptr),
	Cursor(Cursor::Type::Shared, nullptr),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR)),
	Cursor(Cursor::Type::Shared, glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR)),
};

std::vector<Cursor> cursor_cache_custom;

} // namespace


std::reference_wrapper<Cursor> GetCursor(CursorStyle style) {
	if (style >= CursorStyle::_Count) {
		throw std::out_of_range("GetCursor: invalid cursor style");
	}
	return cursor_cache_shared[static_cast<size_t>(style)];
}

std::reference_wrapper<Cursor> CreateCursor(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot) {
	return cursor_cache_custom.emplace_back(Cursor::Type::Custom, GLFW::CreateCursorFromPixelBuffer(pixel_buffer, hotspot));
}


void SetWindowCursor(Handle handle, std::reference_wrapper<Cursor> cursor) {
	switch (cursor.get().type) {
	case Cursor::Type::Hidden:
		glfwSetInputMode(AsGLFWWindow(handle), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case Cursor::Type::NoChange:
		break;
	case Cursor::Type::Shared:
	case Cursor::Type::Custom:
		glfwSetInputMode(AsGLFWWindow(handle), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursor(AsGLFWWindow(handle), cursor.get().handle);
		break;
	}
}


namespace {

inline Point GetWindowCursorPosition(Handle handle) {
	double xpos, ypos;
	glfwGetCursorPos(AsGLFWWindow(handle), &xpos, &ypos);
	return Point(static_cast<int>(xpos), static_cast<int>(ypos));
}

} // namespace


Point GetCursorPosition() {
	glfwSetWindowPos(Context::GetHelperWindow(), 0, 0);
	return GetWindowCursorPosition(Context::GetHelperWindow());
}


} // namespace ViewDesign

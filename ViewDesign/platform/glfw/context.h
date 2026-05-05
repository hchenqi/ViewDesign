#include "ViewDesign/common/type.h"

#include <stdexcept>
#include <string>
#include <iostream>

#include <GLFW/glfw3.h>


namespace ViewDesign {

namespace GLFW {


class Context {
public:
	Context() {
		glfwSetErrorCallback(ErrorCallback);
		if (!glfwInit()) {
			throw std::runtime_error("GLFW: initialization failed");
		}
	}
	~Context() {
		if (helper_window) {
			glfwDestroyWindow(helper_window);
			helper_window = nullptr;
		}
		glfwTerminate();
	}
private:
	static void ErrorCallback(int error, const char* description) {
		try {
			throw std::runtime_error("GLFW: Error " + std::to_string(error) + ": " + description);
		} catch (std::runtime_error& exception) {
			std::cerr << exception.what() << std::endl;
		}
	}
private:
	inline static owner_ptr<GLFWwindow> helper_window = nullptr;
public:
	inline static ref_ptr<GLFWwindow> GetHelperWindow() { return helper_window; }
	inline static bool Initialized() { return helper_window != nullptr; }
private:
	void InitializeWindowHint() {
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	}
public:
	void CreateHelperWindow() {
		if (helper_window == nullptr) {
			InitializeWindowHint();
			helper_window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
			if (helper_window == nullptr) {
				throw std::runtime_error("GLFW: create helper window failed");
			}
		}
		glfwMakeContextCurrent(helper_window);
	}
};


} // namespace GLFW

} // namespace ViewDesign

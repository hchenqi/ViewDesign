#include <GLFW/glfw3.h>

#include <stdexcept>


class GLFWEnvironment {
public:
	GLFWEnvironment() {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}
	}
	~GLFWEnvironment() {
		glfwTerminate();
	}
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}

int main() {
	GLFWEnvironment glfw;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	// glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "GLFWOpenGLTest", nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwSetWindowPos(window, 100, 100);
	glfwShowWindow(window);

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	framebuffer_size_callback(window, 800, 600);

	for (;;) {
		glfwWaitEvents();

		if (glfwWindowShouldClose(window)) {
			glfwDestroyWindow(window);
			break;
		}
	}

	return 0;
}

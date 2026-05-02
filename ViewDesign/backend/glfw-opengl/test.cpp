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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScissor(200, height - 150 - 50, 50, 50);

	{
		auto [r, g, b, a] = std::make_tuple(1.0f, 0.5f, 1.0f, 1.0f);
		glColor4f(r, g, b, a);
	} {
		auto [l, r, t, b] = std::make_tuple(200, 300, 100, 300);
		glBegin(GL_POLYGON);
		glVertex2f(l, t);
		glVertex2f(r, t);
		glVertex2f(r, b);
		glVertex2f(l, b);
		glEnd();
	}

	glfwSwapBuffers(window);
}

int main() {
	GLFWEnvironment glfw;
	// glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SCISSOR_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

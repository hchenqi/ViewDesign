#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/platform/glfw/window.h"
#include "ViewDesign/platform/glfw/timer_queue.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct WindowApi : Window {
	using Window::OnDraw;
};

struct DesktopApi : Desktop {
	using Desktop::window_list;
};

using namespace GLFW;


void EventLoop() {
	std::vector<std::unique_ptr<Window>> window_list_removed;

	for (;;) {
		double timeout = ProcessTimerQueue();
		if (timeout > 0) {
			glfwWaitEventsTimeout(timeout);
		} else {
			glfwWaitEvents();
		}

		auto& window_list = static_cast<DesktopApi&>(desktop).window_list;

		window_list.erase(std::remove_if(window_list.begin(), window_list.end(), [&](auto& window) {
			return glfwWindowShouldClose(AsGLFWWindow(window->GetHandle())) == GLFW_TRUE ? window_list_removed.emplace_back(std::move(window)), true : false;
		}), window_list.end());

		if (window_list.empty()) {
			break;
		}

		for (auto& window : window_list) {
			static_cast<WindowApi&>(*window).OnDraw();
		}
	}
}


} // namespace ViewDesign

#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/platform/glfw/window.h"
#include "ViewDesign/platform/glfw/timer_queue.h"
#include "ViewDesign/view/Desktop.h"


namespace ViewDesign {

struct WindowPrivateAccess : Window {
	using Window::OnDraw;
};

struct DesktopPrivateAccess : Desktop {
	using Desktop::window_list;
};

using namespace GLFW;


void EventLoop() {
	for (;;) {
		double timeout = ProcessTimerQueue();
		if (timeout > 0) {
			glfwWaitEventsTimeout(timeout);
		} else {
			glfwWaitEvents();
		}

		auto& window_list = static_cast<DesktopPrivateAccess&>(desktop.Get()).window_list;

		for (;;) {
			std::vector<std::unique_ptr<Window>> window_closed;
			window_list.erase(std::remove_if(window_list.begin(), window_list.end(), [&](auto& window) {
				return glfwWindowShouldClose(AsGLFWWindow(window->GetHandle())) == GLFW_TRUE ? window_closed.emplace_back(std::move(window)), true : false;
			}), window_list.end());
			if (window_closed.empty()) { break; }
		}

		if (window_list.empty()) {
			break;
		}

		for (auto& window : window_list) {
			static_cast<WindowPrivateAccess&>(*window).OnDraw();
		}
	}
}


} // namespace ViewDesign

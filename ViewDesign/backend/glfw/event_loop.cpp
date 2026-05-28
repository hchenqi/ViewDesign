#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/view/Desktop.h"

#include <ViewDesign/platform/glfw/window.h>
#include <ViewDesign/platform/glfw/timer_queue.h>


namespace ViewDesign {

struct WindowPrivateAccess : Window {
	using Window::Draw;
};

struct DesktopPrivateAccess : Desktop {
	using Desktop::window_list;
};

using namespace GLFW;


namespace {

inline void CheckDestroyWindows() {
	for (auto& window_list = static_cast<DesktopPrivateAccess&>(desktop.Get()).window_list;;) {
		std::vector<std::unique_ptr<Window>> window_to_destroy;
		window_list.erase(std::remove_if(window_list.begin(), window_list.end(), [&](auto& window) {
			return glfwWindowShouldClose(AsGLFWWindow(window->GetHandle())) == GLFW_TRUE ? window_to_destroy.emplace_back(std::move(window)), true : false;
		}), window_list.end());
		if (window_to_destroy.empty()) {
			break;
		}
	}
}

inline void DrawWindows() {
	for (auto& window : static_cast<DesktopPrivateAccess&>(desktop.Get()).window_list) {
		static_cast<WindowPrivateAccess&>(*window).Draw();
	}
}

} // namespace


void EventLoop() {
	for (;;) {
		double timeout = ProcessTimerQueue();
		if (timeout > 0) {
			glfwWaitEventsTimeout(timeout);
		} else {
			glfwWaitEvents();
		}

		CheckDestroyWindows();
		DrawWindows();

		if (desktop.WindowListEmpty()) {
			break;
		}
	}
}

void PollEvents() {
	ProcessTimerQueue();
	glfwPollEvents();
	CheckDestroyWindows();
	DrawWindows();
}


} // namespace ViewDesign

#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/view/Desktop.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {

struct WindowApi : Window {
	using Window::OnDraw;
};

struct DesktopApi : Desktop {
	using Desktop::window_list;
};


void EventLoop() {
	for (;;) {
		glfwWaitEvents();
		auto& window_list = static_cast<DesktopApi&>(desktop).window_list;
		if (window_list.empty()) {
			break;
		}
		for (auto& window : window_list) {
			static_cast<WindowApi&>(*window).OnDraw();
		}
	}
}

void Terminate() {}


} // namespace ViewDesign

#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>


namespace ViewDesign {

struct DesktopApi : Desktop {
	using Desktop::window_list;
};


void EventLoop() {
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if (static_cast<DesktopApi&>(desktop).window_list.empty()) {
			PostQuitMessage(0);
		}
	}
}


} // namespace ViewDesign

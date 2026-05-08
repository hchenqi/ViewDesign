#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>


namespace ViewDesign {

struct DesktopPrivateAccess : Desktop {
	using Desktop::window_list;
};


void EventLoop() {
	MSG msg;
	for (;;) {
		GetMessageW(&msg, nullptr, 0, 0);
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if (static_cast<DesktopPrivateAccess&>(desktop.Get()).window_list.empty()) {
			break;
		}
	}
}


} // namespace ViewDesign

#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>


namespace ViewDesign {


void EventLoop() {
	for (MSG msg;;) {
		GetMessageW(&msg, nullptr, 0, 0);
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if (desktop.WindowListEmpty()) {
			break;
		}
	}
}

void PollEvents() {
	for (MSG msg; PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}


} // namespace ViewDesign

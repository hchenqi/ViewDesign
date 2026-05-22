#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/view/Desktop.h"

#include <windows.h>


namespace ViewDesign {


void EventLoop() {
	MSG msg;
	for (;;) {
		GetMessageW(&msg, nullptr, 0, 0);
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if (desktop.WindowListEmpty()) {
			break;
		}
	}
}


} // namespace ViewDesign

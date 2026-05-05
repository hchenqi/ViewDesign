#include "ViewDesign/system/event_loop.h"

#include <windows.h>


namespace ViewDesign {


void EventLoop() {
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}


} // namespace ViewDesign

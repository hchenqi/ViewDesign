#include "ViewDesign/view/Desktop.h"

#include <Windows.h>


extern void main();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	main();
	ViewDesign::desktop.Terminate();
	return 0;
}

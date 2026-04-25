#include "ViewDesign/view/Desktop.h"

#include <windows.h>


void App();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	App();
	ViewDesign::desktop.Terminate();
	return 0;
}

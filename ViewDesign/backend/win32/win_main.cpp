#include "ViewDesign/view/Desktop.h"

#include <Windows.h>


extern int main();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	int ret = main();
	ViewDesign::desktop.Terminate();
	return ret;
}

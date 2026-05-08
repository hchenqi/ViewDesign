#include "ViewDesign/platform/win32/opengl_context.h"

#include <glad/glad.h>


void App();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	ViewDesign::Win32::OpenGLContext context;
	gladLoadGL();
	App();
	return 0;
}

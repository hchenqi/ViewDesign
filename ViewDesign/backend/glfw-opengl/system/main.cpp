#include "ViewDesign/view/Desktop.h"

#include <glad/glad.h>
#include "ViewDesign/platform/glfw/context.h"


void App();

int main() {
	ViewDesign::GLFW::Context context;
	context.CreateHelperWindow();
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	App();
	ViewDesign::desktop.Terminate();
	return 0;
}


#if defined(VIEWDESIGN_PLATFORM_WIN32)

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) { return main(); }

#endif

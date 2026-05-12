#define VK_USE_PLATFORM_WIN32_KHR
#include "ViewDesign/platform/vulkan/instance.h"
#include "ViewDesign/platform/vulkan/debug_messenger.h"

#include <windows.h>


void App();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	ViewDesign::Vulkan::InstanceContext::Initialize(
		{
			// "VK_LAYER_KHRONOS_validation"
		}, {
			vk::EXTDebugUtilsExtensionName,
			vk::KHRSurfaceExtensionName,
			vk::KHRWin32SurfaceExtensionName
		}
	);
	ViewDesign::Vulkan::DebugMessengerContext::Initialize();
	App();
	return 0;
}

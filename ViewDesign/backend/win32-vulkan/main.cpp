#define VK_USE_PLATFORM_WIN32_KHR
#include "ViewDesign/platform/vulkan/instance.h"
#include "ViewDesign/platform/vulkan/debug_messenger.h"

#include <windows.h>


void App();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	ViewDesign::Vulkan::InstanceContext::Initialize(
		{
#if !defined(NDEBUG)
			"VK_LAYER_KHRONOS_validation"
#endif
		}, {
#if !defined(NDEBUG)
			vk::EXTDebugUtilsExtensionName,
#endif
			vk::KHRSurfaceExtensionName,
			vk::KHRWin32SurfaceExtensionName
		}
	);
#if !defined(NDEBUG)
	ViewDesign::Vulkan::DebugMessengerContext::Initialize();
#endif
	App();
	return 0;
}

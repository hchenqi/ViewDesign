#include <ViewDesign/platform/vulkan/instance.h>
#include <ViewDesign/platform/vulkan/debug_messenger.h>
#include <ViewDesign/platform/glfw/context.h>


void App();

int main() {
	ViewDesign::GLFW::Context context;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	context.CreateHelperWindow();

	uint32_t count = 0;
	const char** list = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*> extension_list(list, list + count);
#if !defined(NDEBUG)
	extension_list.emplace_back(vk::EXTDebugUtilsExtensionName);
#endif

	ViewDesign::Vulkan::InstanceContext::Initialize(
		{
#if !defined(NDEBUG)
			"VK_LAYER_KHRONOS_validation"
#endif
		}, extension_list
	);
#if !defined(NDEBUG)
	ViewDesign::Vulkan::DebugMessengerContext::Initialize();
#endif

	App();
	return 0;
}


#if defined(VIEWDESIGN_PLATFORM_WIN32)

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) { return main(); }

#endif

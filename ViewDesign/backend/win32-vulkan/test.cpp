#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_raii.hpp>
#include <array>
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <vector>

static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

// ─── Win32Window ──────────────────────────────────────────────────────────────

class Win32Window {
public:
	HINSTANCE instance_handle;
	HWND      window_handle = nullptr;
	bool      should_close = false;

	Win32Window(uint32_t width, uint32_t height, const char* title) {
		instance_handle = GetModuleHandleW(nullptr);
		if (instance_count++ == 0) {
			WNDCLASSEXW window_class{};
			window_class.cbSize = sizeof(window_class);
			window_class.style = CS_HREDRAW | CS_VREDRAW;
			window_class.lpfnWndProc = WindowProc;
			window_class.hInstance = instance_handle;
			window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			window_class.lpszClassName = L"VkRaiiDemo";
			RegisterClassExW(&window_class);
		}

		RECT window_rect{ 0, 0, (LONG)width, (LONG)height };
		AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);
		window_handle = CreateWindowExW(0, L"VkRaiiDemo", std::wstring(title, title + strlen(title)).c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, nullptr, nullptr, instance_handle, this);
		if (!window_handle) { throw std::runtime_error("CreateWindowEx failed"); }
		ShowWindow(window_handle, SW_SHOW);
	}

	~Win32Window() {
		if (window_handle) { DestroyWindow(window_handle); }
		if (--instance_count == 0) { UnregisterClassW(L"VkRaiiDemo", GetModuleHandleW(nullptr)); }
	}

	Win32Window(const Win32Window&) = delete;
	Win32Window& operator=(const Win32Window&) = delete;

	void PollEvents() {
		MSG message{};
		while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

private:
	inline static uint32_t instance_count = 0;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		Win32Window* self = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		if (message == WM_NCCREATE) {
			self = reinterpret_cast<Win32Window*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		}
		if (message == WM_CLOSE) { self->should_close = true; DestroyWindow(hwnd); return 0; }
		if (message == WM_DESTROY) { self->window_handle = nullptr; return 0; }
		return DefWindowProcW(hwnd, message, wparam, lparam);
	}
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* data, void*) {
	if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) { fprintf(stderr, "[Vulkan] %s\n", data->pMessage); }
	return VK_FALSE;
}

// ─── VulkanDevice ─────────────────────────────────────────────────────────────
// Shared across all windows.
// Selects a GPU with a graphics queue + swapchain support; present capability is verified per-surface inside VulkanWindow.

struct VulkanDevice {
	vk::raii::Context                context;
	vk::raii::Instance               instance{ nullptr };
	vk::raii::DebugUtilsMessengerEXT debug_messenger{ nullptr };
	vk::raii::PhysicalDevice         physical_device{ nullptr };
	uint32_t                         graphics_family_index{};
	vk::raii::Device                 device{ nullptr };
	vk::raii::Queue                  graphics_queue{ nullptr };

	VulkanDevice() {
		vk::ApplicationInfo application_info("VkRaiiDemo", 1, nullptr, 0, VK_API_VERSION_1_2);
		std::vector<const char*> instance_layers = {
			//"VK_LAYER_KHRONOS_validation"
		};
		std::vector<const char*> instance_extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
		instance = context.createInstance(vk::InstanceCreateInfo({}, &application_info, instance_layers, instance_extensions));

		using Severity = vk::DebugUtilsMessageSeverityFlagBitsEXT;
		using Type = vk::DebugUtilsMessageTypeFlagBitsEXT;
		debug_messenger = instance.createDebugUtilsMessengerEXT(vk::DebugUtilsMessengerCreateInfoEXT({}, Severity::eVerbose | Severity::eWarning | Severity::eError, Type::eGeneral | Type::eValidation | Type::ePerformance, DebugCallback));

		for (auto& candidate : vk::raii::PhysicalDevices(instance)) {
			bool has_swapchain = false;
			for (const auto& extension : candidate.enumerateDeviceExtensionProperties()) {
				if (std::string_view(extension.extensionName.data()) == VK_KHR_SWAPCHAIN_EXTENSION_NAME) { has_swapchain = true; break; }
			}
			if (!has_swapchain) { continue; }

			std::vector<vk::QueueFamilyProperties> queue_family_properties_list = candidate.getQueueFamilyProperties();
			for (uint32_t i = 0; i < queue_family_properties_list.size(); ++i) {
				if (queue_family_properties_list[i].queueFlags & vk::QueueFlagBits::eGraphics) {
					graphics_family_index = i;
					physical_device = std::move(candidate);
					break;
				}
			}
			if (*physical_device) { break; }
		}
		if (!*physical_device) { throw std::runtime_error("Vulkan: no suitable GPU found"); }

		float queue_priority = 1.0f;
		vk::DeviceQueueCreateInfo queue_create_info({}, graphics_family_index, 1, &queue_priority);
		std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		device = physical_device.createDevice(vk::DeviceCreateInfo({}, queue_create_info, {}, extensions));
		graphics_queue = device.getQueue(graphics_family_index, 0);
	}

	VulkanDevice(const VulkanDevice&) = delete;
	VulkanDevice& operator=(const VulkanDevice&) = delete;
};

// ─── VulkanWindow ─────────────────────────────────────────────────────────────
// All per-window resources. win32_window is declared first so it is destroyed
// last — the HWND must outlive the Vulkan surface that wraps it.

struct VulkanWindow {
	Win32Window                        win32_window;
	vk::raii::SurfaceKHR               surface{ nullptr };
	vk::SurfaceFormatKHR               swapchain_surface_format;
	vk::Extent2D                       swapchain_extent;
	vk::raii::SwapchainKHR             swapchain{ nullptr };
	std::vector<vk::raii::ImageView>   swapchain_image_views;
	vk::raii::RenderPass               render_pass{ nullptr };
	std::vector<vk::raii::Framebuffer> framebuffers;
	vk::raii::CommandPool              command_pool{ nullptr };
	vk::raii::CommandBuffers           command_buffers{ nullptr };
	std::vector<vk::raii::Semaphore>   image_available_semaphores;
	std::vector<vk::raii::Semaphore>   render_finished_semaphores;
	std::vector<vk::raii::Fence>       in_flight_fences;
	vk::ClearColorValue                clear_color;
	uint32_t                           current_frame_index = 0;

	VulkanWindow(const VulkanDevice& vulkan_device, uint32_t width, uint32_t height, const char* title, vk::ClearColorValue clear_color_value)
		: win32_window(width, height, title), clear_color(clear_color_value) {
		surface = vulkan_device.instance.createWin32SurfaceKHR(vk::Win32SurfaceCreateInfoKHR({}, win32_window.instance_handle, win32_window.window_handle));

		// Verify the single shared queue family can present to this surface.
		// On desktop hardware the graphics queue always supports present.
		if (!vulkan_device.physical_device.getSurfaceSupportKHR(vulkan_device.graphics_family_index, *surface)) {
			throw std::runtime_error("Vulkan: graphics queue does not support present on this surface");
		}

		// ── Swapchain ─────────────────────────────────────────────────────────
		vk::SurfaceCapabilitiesKHR        surface_capabilities = vulkan_device.physical_device.getSurfaceCapabilitiesKHR(*surface);
		std::vector<vk::SurfaceFormatKHR> surface_formats = vulkan_device.physical_device.getSurfaceFormatsKHR(*surface);
		std::vector<vk::PresentModeKHR>   present_modes = vulkan_device.physical_device.getSurfacePresentModesKHR(*surface);

		swapchain_surface_format = surface_formats[0];
		for (const auto& format : surface_formats) {
			if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) { swapchain_surface_format = format; break; }
		}

		vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo;
		for (const auto& mode : present_modes) {
			if (mode == vk::PresentModeKHR::eMailbox) { present_mode = mode; break; }
		}

		swapchain_extent = surface_capabilities.currentExtent.width != UINT32_MAX
			? surface_capabilities.currentExtent
			: vk::Extent2D{ std::clamp(width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width), std::clamp(height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height) };

		uint32_t swapchain_image_count = std::min(surface_capabilities.minImageCount + 1, surface_capabilities.maxImageCount > 0 ? surface_capabilities.maxImageCount : UINT32_MAX);

		// Single queue family handles both graphics and present, so eExclusive is always correct here.
		swapchain = vulkan_device.device.createSwapchainKHR(vk::SwapchainCreateInfoKHR(
			{}, *surface, swapchain_image_count,
			swapchain_surface_format.format, swapchain_surface_format.colorSpace,
			swapchain_extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive, {},
			surface_capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
			present_mode, VK_TRUE));

		// ── Image views ───────────────────────────────────────────────────────
		for (const vk::Image image : swapchain.getImages()) {
			swapchain_image_views.emplace_back(vulkan_device.device.createImageView(vk::ImageViewCreateInfo({}, image, vk::ImageViewType::e2D, swapchain_surface_format.format, {}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 })));
		}

		// ── Render pass ───────────────────────────────────────────────────────
		vk::AttachmentDescription color_attachment(
			{}, swapchain_surface_format.format, vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		vk::AttachmentReference color_attachment_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::SubpassDescription  subpass({}, vk::PipelineBindPoint::eGraphics, {}, color_attachment_reference);
		vk::SubpassDependency   subpass_dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);
		render_pass = vulkan_device.device.createRenderPass(vk::RenderPassCreateInfo({}, color_attachment, subpass, subpass_dependency));

		// ── Framebuffers ──────────────────────────────────────────────────────
		for (const auto& image_view : swapchain_image_views) {
			framebuffers.emplace_back(vulkan_device.device.createFramebuffer(vk::FramebufferCreateInfo({}, *render_pass, *image_view, swapchain_extent.width, swapchain_extent.height, 1)));
		}

		// ── Command pool + buffers ────────────────────────────────────────────
		command_pool = vulkan_device.device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, vulkan_device.graphics_family_index));
		command_buffers = vk::raii::CommandBuffers(vulkan_device.device, vk::CommandBufferAllocateInfo(*command_pool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT));

		// ── Sync objects ──────────────────────────────────────────────────────
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			image_available_semaphores.emplace_back(vulkan_device.device.createSemaphore({}));
			render_finished_semaphores.emplace_back(vulkan_device.device.createSemaphore({}));
			in_flight_fences.emplace_back(vulkan_device.device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)));
		}
	}

	VulkanWindow(const VulkanWindow&) = delete;
	VulkanWindow& operator=(const VulkanWindow&) = delete;

	void RecordCommands(vk::raii::CommandBuffer& command_buffer, uint32_t image_index) {
		command_buffer.begin(vk::CommandBufferBeginInfo{});
		vk::ClearValue clear_value(clear_color);
		command_buffer.beginRenderPass(
			vk::RenderPassBeginInfo(
				*render_pass, *framebuffers[image_index],
				vk::Rect2D({ 0, 0 }, swapchain_extent),
				clear_value),
			vk::SubpassContents::eInline);
		// ← draw calls here
		command_buffer.endRenderPass();
		command_buffer.end();
	}

	void DrawFrame(const VulkanDevice& vulkan_device) {
		auto wait_result = vulkan_device.device.waitForFences(*in_flight_fences[current_frame_index], VK_TRUE, UINT64_MAX);
		(void)wait_result;
		vulkan_device.device.resetFences(*in_flight_fences[current_frame_index]);

		auto [acquire_result, image_index] = swapchain.acquireNextImage(UINT64_MAX, *image_available_semaphores[current_frame_index]);
		if (acquire_result == vk::Result::eErrorOutOfDateKHR) { return; } // TODO: recreate swapchain

		command_buffers[current_frame_index].reset();
		RecordCommands(command_buffers[current_frame_index], image_index);

		vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vulkan_device.graphics_queue.submit(vk::SubmitInfo(*image_available_semaphores[current_frame_index], wait_stage, *command_buffers[current_frame_index], *render_finished_semaphores[current_frame_index]), *in_flight_fences[current_frame_index]);

		auto present_result = vulkan_device.graphics_queue.presentKHR(vk::PresentInfoKHR(*render_finished_semaphores[current_frame_index], *swapchain, image_index));
		(void)present_result;

		current_frame_index = (current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
	}
};

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
	try {
		VulkanDevice vulkan_device;

		// Both windows share the same VulkanDevice (instance, physical device,
		// logical device, queue). Each gets its own surface + swapchain + render
		// resources. Close either window to exit; the loop stops when both are closed.
		VulkanWindow window_1(vulkan_device, 1280, 720, "Window 1", vk::ClearColorValue(std::array<float, 4>{ 0.05f, 0.25f, 0.28f, 1.0f })); // teal
		VulkanWindow window_2(vulkan_device, 1280, 720, "Window 2", vk::ClearColorValue(std::array<float, 4>{ 0.28f, 0.05f, 0.25f, 1.0f })); // purple

		while (!window_1.win32_window.should_close || !window_2.win32_window.should_close) {
			window_1.win32_window.PollEvents();
			window_2.win32_window.PollEvents();
			if (!window_1.win32_window.should_close) { window_1.DrawFrame(vulkan_device); }
			if (!window_2.win32_window.should_close) { window_2.DrawFrame(vulkan_device); }
		}

		// Wait for all GPU work to finish before the windows and device go out of scope.
		vulkan_device.device.waitIdle();
	} catch (const vk::SystemError& error) {
		MessageBoxA(nullptr, error.what(), "Vulkan Error", MB_OK | MB_ICONERROR);
		return 1;
	} catch (const std::exception& error) {
		MessageBoxA(nullptr, error.what(), "Fatal Error", MB_OK | MB_ICONERROR);
		return 1;
	}
	return 0;
}

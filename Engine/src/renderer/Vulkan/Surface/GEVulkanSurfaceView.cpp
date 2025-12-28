#include <iostream>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>	
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "renderer/Vulkan/Surface/GEVulkanSurfaceView.h"

void GEVulkanSurfaceView::CreateSurface(WindowCreator* window)
{
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hinstance = GetModuleHandle(nullptr);
	createInfo.hwnd = glfwGetWin32Window(window->GetWindowHandle());

	if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surfaceView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void GEVulkanSurfaceView::Cleanup()
{
	if (m_surfaceView != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_instance, m_surfaceView, nullptr);
		m_surfaceView = VK_NULL_HANDLE;
	}
}

#pragma once

#include "vulkan/vulkan.h"
#include "platform/Window.h"

class GEVulkanSurfaceView {

private:
	VkSurfaceKHR m_surfaceView;
	VkInstance m_instance;
public:
	GEVulkanSurfaceView(const GEVulkanSurfaceView& other)
		: m_surfaceView(other.m_surfaceView),
		m_instance(other.m_instance) {
	}

	GEVulkanSurfaceView(VkInstance instance) : m_instance(instance) { m_surfaceView = VK_NULL_HANDLE; }
	~GEVulkanSurfaceView() = default;

	void CreateSurface(WindowCreator* window);
	VkSurfaceKHR getSurfaceView() { return m_surfaceView; };

	void Cleanup();
};
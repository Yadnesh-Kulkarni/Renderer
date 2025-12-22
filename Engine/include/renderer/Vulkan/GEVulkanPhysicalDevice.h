#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Vulkan/GEVulkanSurfaceView.h"
#include "renderer/Vulkan/common/GEVulkancommon.h"

class GEVulkanPhysicalDevice {

private:
	GEVulkanSurfaceView m_vkSurfaceView;
	VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_queueFamilyIndices {};

	int rateDeviceSuitablity(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices& indices);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

public:
	
    GEVulkanPhysicalDevice(const GEVulkanPhysicalDevice& other)
        : m_vkSurfaceView(other.m_vkSurfaceView),
          m_vkPhysicalDevice(other.m_vkPhysicalDevice),
          m_queueFamilyIndices(other.m_queueFamilyIndices) {}

	GEVulkanPhysicalDevice(const GEVulkanSurfaceView& view) : m_vkSurfaceView(view) {};
	~GEVulkanPhysicalDevice() = default;

	void pickPhysicalDevice(VkInstance instance);
	const VkPhysicalDevice GetPhysicalDevice() { return m_vkPhysicalDevice; }

	const QueueFamilyIndices GetQueueFamilyIndices() { return m_queueFamilyIndices; }
	void Cleanup();
};
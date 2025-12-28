#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Vulkan/Surface/GEVulkanSurfaceView.h"
#include "renderer/Vulkan/common/GEVulkancommon.h"

class GEVulkanPhysicalDevice {

private:
	GEVulkanSurfaceView m_vkSurfaceView;
	VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;

	GEPhysicalDeviceDetails m_deviceDetails{};

	int rateDeviceSuitablity(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device, GEPhysicalDeviceDetails& indices);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
public:
	
    GEVulkanPhysicalDevice(const GEVulkanPhysicalDevice& other)
        : m_vkSurfaceView(other.m_vkSurfaceView),
          m_vkPhysicalDevice(other.m_vkPhysicalDevice) {}

	GEVulkanPhysicalDevice(const GEVulkanSurfaceView& view) : m_vkSurfaceView(view) {};
	~GEVulkanPhysicalDevice() = default;

	uint32_t GetDeviceExtensionCount() { return static_cast<uint32_t>(deviceExtensions.size()); }
	const char* const* GetDeviceExtensions() { return deviceExtensions.data(); }


	void pickPhysicalDevice(VkInstance instance);
	const VkPhysicalDevice GetPhysicalDevice() { return m_vkPhysicalDevice; }

	const QueueFamilyIndices GetQueueFamilyIndices() { return m_deviceDetails.queueFamilyIndices; }
	const SwapchainSupportDetails GetSwapchainSupportDetails() { return m_deviceDetails.swapchainSupportDetails; }
	void Cleanup();
};
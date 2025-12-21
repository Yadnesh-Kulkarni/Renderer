#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Vulkan/common/GEVulkancommon.h"

class GEVulkanPhysicalDevice {

private:
	VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
	int rateDeviceSuitablity(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
public:
	GEVulkanPhysicalDevice() {} ;
	~GEVulkanPhysicalDevice() = default;

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void pickPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice);
	VkPhysicalDevice GetPhysicalDevice() { return m_vkPhysicalDevice; }

	void Cleanup() {
		m_vkPhysicalDevice = VK_NULL_HANDLE;
	}
};
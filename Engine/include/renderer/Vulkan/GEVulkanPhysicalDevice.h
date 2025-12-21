#pragma once
#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
};

class GEVulkanPhysicalDevice {

private:
	VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
	int rateDeviceSuitablity(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
public:
	GEVulkanPhysicalDevice() {} ;
	~GEVulkanPhysicalDevice() = default;

	void pickPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice);
	VkPhysicalDevice GetPhysicalDevice() { return m_vkPhysicalDevice; }
};
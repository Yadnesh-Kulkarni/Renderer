#pragma once
#include <Vulkan/vulkan.h>
#include "renderer/Vulkan/GEVulkanPhysicalDevice.h"

class GEVulkanLogicalDevice{
private:
	VkDevice m_vkDevice = VK_NULL_HANDLE;
	VkQueue m_vkGraphicsQueue = VK_NULL_HANDLE;
	GEVulkanPhysicalDevice* m_vkPhysicalDevice;
public:
	GEVulkanLogicalDevice(GEVulkanPhysicalDevice* device) : m_vkPhysicalDevice(device) { m_vkDevice = VK_NULL_HANDLE; }
	~GEVulkanLogicalDevice() = default;

	void createLogicalDevice();
	void cleanUp();
	VkDevice& getVkDevice() { return m_vkDevice; }
	VkQueue& getVkQueue() { return m_vkGraphicsQueue; }
};
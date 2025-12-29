#pragma once
#include <vulkan/vulkan.h>

class GEVulkanCommandPool {
private:
	VkCommandPool m_vkCommandPool = VK_NULL_HANDLE;

public:
	GEVulkanCommandPool() {};
	~GEVulkanCommandPool() = default;

	void CreateCommandPool(VkDevice device, uint32_t queueFamilyIndex);
	VkCommandPool GetCommandPool() const { return m_vkCommandPool; }

	void Cleanup(VkDevice device);
};
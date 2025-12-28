#pragma once
#include <vulkan/vulkan.h>

class GEVulkanRenderPass {
private:
	VkDevice m_device = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkFormat m_swapChainImageFormat;
public:
	GEVulkanRenderPass(const GEVulkanRenderPass& other) {
		m_device = other.m_device;
		m_renderPass = other.m_renderPass;
		m_swapChainImageFormat = other.m_swapChainImageFormat;
	}

	GEVulkanRenderPass(VkDevice& device, VkFormat format) : m_device(device), m_swapChainImageFormat(format) {};
	~GEVulkanRenderPass() = default;
	void CreateRenderPass();
	VkRenderPass& GetRenderPass() { return m_renderPass; }
	void Cleanup();
};
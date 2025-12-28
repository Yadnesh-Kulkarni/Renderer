#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "GEVulkanRenderPass.h"

class GEVulkanFramebuffer {
private:
	VkFramebuffer m_vkFramebuffer = VK_NULL_HANDLE;
public:
	GEVulkanFramebuffer(const GEVulkanFramebuffer& other) {
		m_vkFramebuffer = other.m_vkFramebuffer;
	};

	GEVulkanFramebuffer() {};
	~GEVulkanFramebuffer() = default;

	void CreateFramebuffer(VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkImageView imageView);
	void Cleanup(VkDevice device);

	VkFramebuffer GetVkFramebuffer() const { return m_vkFramebuffer; }
};
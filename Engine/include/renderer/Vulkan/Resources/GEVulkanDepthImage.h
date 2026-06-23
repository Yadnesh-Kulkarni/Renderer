#pragma once
#include <vulkan/vulkan.h>

// Depth buffer texture: attachment-only (no sampling), sized to match the framebuffer extent.
class GEVulkanDepthImage {
private:
	VkImage m_image = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkImageView m_imageView = VK_NULL_HANDLE;
	VkFormat m_format = VK_FORMAT_UNDEFINED;

	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

public:
	GEVulkanDepthImage(const GEVulkanDepthImage& other)
	{
		m_image = other.m_image;
		m_memory = other.m_memory;
		m_imageView = other.m_imageView;
		m_format = other.m_format;
	}

	GEVulkanDepthImage() = default;
	~GEVulkanDepthImage() = default;

	void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat format);
	void Cleanup(VkDevice device);

	VkImageView GetImageView() const { return m_imageView; }
	VkFormat GetFormat() const { return m_format; }
};

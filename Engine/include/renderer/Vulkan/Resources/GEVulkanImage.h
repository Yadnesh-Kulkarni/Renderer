#pragma once

#include <vulkan/vulkan.h>

class GEVulkanImage {
public:
	GEVulkanImage() = default;
	~GEVulkanImage() = default;

	void Create(VkPhysicalDevice physicalDevice, VkDevice device,
		uint32_t width, uint32_t height, VkFormat format, uint32_t mipLevels = 1);

	void Cleanup(VkDevice device);

	VkImage GetImage() const { return m_image; }
	VkImageView GetImageView() const { return m_imageView; }
	VkFormat GetFormat() const { return m_format; }
	VkExtent2D GetExtent() const { return m_extent; }
	uint32_t GetMipLevels() const { return m_mipLevels; }
	VkImageLayout GetLayout() const { return m_layout; }
	void SetLayout(VkImageLayout layout) { m_layout = layout; }

private:
	VkImage m_image = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkImageView m_imageView = VK_NULL_HANDLE;
	VkFormat m_format = VK_FORMAT_UNDEFINED;
	VkExtent2D m_extent{};
	uint32_t m_mipLevels = 1;
	VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

	static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,
		uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

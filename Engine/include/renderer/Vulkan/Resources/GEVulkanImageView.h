#pragma once
#include <vulkan/vulkan.h>

class GEVulkanImageView {
private:
	VkImageView m_imageView; 
	
public:
	GEVulkanImageView() {m_imageView = VK_NULL_HANDLE;}
	~GEVulkanImageView() = default;

	GEVulkanImageView CreateImageView(VkDevice device, VkImage image, VkFormat format);
	VkImageView GetImageView() const { return m_imageView; }
	void Cleanup(VkDevice device);
};
#include <iostream>
#include "renderer/Vulkan/Resources/GEVulkanImageView.h"

GEVulkanImageView GEVulkanImageView::CreateImageView(VkDevice device, VkImage image, VkFormat format)
{
	VkImageViewCreateInfo viewInfo{};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.image = image;
	viewInfo.format = format;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create image views!");
	}

	return *this;
}

void GEVulkanImageView::Cleanup(VkDevice device)
{
	if (m_imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device, m_imageView, nullptr);
		m_imageView = VK_NULL_HANDLE;
	}
}

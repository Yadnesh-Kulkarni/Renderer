#include <stdexcept>
#include "renderer/Vulkan/Resources/GEVulkanFramebuffer.h"

void GEVulkanFramebuffer::CreateFramebuffer(VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkImageView imageView)
{
	VkFramebufferCreateInfo framebufferInfo{};

	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &imageView;
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_vkFramebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create framebuffer!");
	}
}

void GEVulkanFramebuffer::Cleanup(VkDevice device)
{
	if(m_vkFramebuffer)
	{
		
	vkDestroyFramebuffer(device, m_vkFramebuffer, nullptr);
	}
}

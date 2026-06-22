#include <array>
#include <stdexcept>
#include "renderer/Vulkan/Resources/GEVulkanFramebuffer.h"

void GEVulkanFramebuffer::CreateFramebuffer(
	VkDevice device,
	VkExtent2D extent,
	VkRenderPass renderPass,
	VkImageView colorImageView,
	VkImageView depthImageView)
{
	std::array attachments = { colorImageView, depthImageView };

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
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
	if (m_vkFramebuffer)
	{
		vkDestroyFramebuffer(device, m_vkFramebuffer, nullptr);
	}
}

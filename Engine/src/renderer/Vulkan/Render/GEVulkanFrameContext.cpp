#include <memory>
#include <stdexcept>
#include "renderer/Vulkan/Render/GEVulkanFrameContext.h"

void GEVulkanFrameContext::CreateFrameContext()
{
	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	m_viewport.width = static_cast<float>(m_width);
	m_viewport.height = static_cast<float>(m_height);
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	m_scissor.offset = { 0, 0 };
	m_scissor.extent = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) };
}

void GEVulkanFrameContext::CreateCmdBuffer(VkCommandPool commandPool, VkDevice device)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	if (vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

#include <stdexcept>
#include "renderer/Vulkan/Render/GEVulkanFrameContext.h"

GEVulkanFrameContext::GEVulkanFrameContext(VkDevice device, int width, int height)
	: m_device(device), m_width(width), m_height(height)
{
	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	m_viewport.width = static_cast<float>(m_width);
	m_viewport.height = static_cast<float>(m_height);
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	m_scissor.offset = { 0, 0 };
	m_scissor.extent = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) };

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void GEVulkanFrameContext::Cleanup()
{
	for (size_t i = 0; i < m_inFlightFences.size(); i++)
	{
		if (m_inFlightFences[i] != VK_NULL_HANDLE)
		{
			vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
			m_inFlightFences[i] = VK_NULL_HANDLE;
		}
	}

	for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++)
	{
		if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
			m_imageAvailableSemaphores[i] = VK_NULL_HANDLE;
		}
	}
}

void GEVulkanFrameContext::CreateCmdBuffers(VkCommandPool commandPool)
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void GEVulkanFrameContext::WaitForCurrentFrameFence()
{
	vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
}

void GEVulkanFrameContext::ResetCurrentFrameFence()
{
	vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
}

void GEVulkanFrameContext::AdvanceFrame()
{
	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void GEVulkanFrameContext::UpdateExtent(uint32_t width, uint32_t height)
{
	m_width  = static_cast<int>(width);
	m_height = static_cast<int>(height);
	m_viewport.y      = 0.0f;
	m_viewport.width  = static_cast<float>(width);
	m_viewport.height = static_cast<float>(height);
	m_scissor.extent  = { width, height };
}

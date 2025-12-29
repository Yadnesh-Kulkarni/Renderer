#pragma once
#include <vulkan/vulkan.h>

class GEVulkanFrameContext {
private:
	VkViewport m_viewport;
	VkRect2D m_scissor;
	VkCommandBuffer	m_commandBuffer;

	int m_width;
	int m_height;

public:
	GEVulkanFrameContext(const GEVulkanFrameContext& other) {
		m_width = other.m_width;
		m_height = other.m_height;
		m_viewport = other.m_viewport;
		m_scissor = other.m_scissor;
	}

	GEVulkanFrameContext(int width,int height) : m_width(width), m_height(height) {
		m_viewport = {};
		m_scissor = {};
	};

	~GEVulkanFrameContext() = default;

	void CreateFrameContext();
	void CreateCmdBuffer(VkCommandPool commandPool, VkDevice device);
	VkViewport& GetViewport() { return m_viewport; }
	VkRect2D& GetScissor() { return m_scissor; }

	VkCommandBuffer& GetCommandBuffer() { return m_commandBuffer; }
};
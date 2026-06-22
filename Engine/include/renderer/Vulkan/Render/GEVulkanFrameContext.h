#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class GEVulkanFrameContext {
public:
	static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

private:
	VkDevice m_device = VK_NULL_HANDLE;
	VkViewport m_viewport;
	VkRect2D m_scissor;

	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkFence> m_inFlightFences;
	size_t m_currentFrame = 0;

	int m_width;
	int m_height;

public:
	GEVulkanFrameContext(const GEVulkanFrameContext& other) = default;

	GEVulkanFrameContext(VkDevice device, int width, int height);
	~GEVulkanFrameContext() = default;

	void Cleanup();

	void CreateCmdBuffers(VkCommandPool commandPool);
	void WaitForCurrentFrameFence();
	void ResetCurrentFrameFence();
	void AdvanceFrame();
	void UpdateExtent(uint32_t width, uint32_t height);

	VkViewport& GetViewport() { return m_viewport; }
	VkRect2D& GetScissor() { return m_scissor; }

	VkCommandBuffer GetCommandBuffer() const { return m_commandBuffers[m_currentFrame]; }

	VkFence GetInFlightFence() const { return m_inFlightFences[m_currentFrame]; }
	VkSemaphore GetImageAvailableSemaphore() const { return m_imageAvailableSemaphores[m_currentFrame]; }
};

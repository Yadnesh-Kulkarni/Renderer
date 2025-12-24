#include <vulkan/vulkan.h>

class GEVulkanRenderPass {
private:
	VkDevice m_device = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkFormat m_swapChainImageFormat;
public:
	GEVulkanRenderPass(VkDevice& device, VkFormat format) : m_device(device), m_swapChainImageFormat(format) {};
	~GEVulkanRenderPass() = default;
	void CreateRenderPass();
	void Cleanup();
};
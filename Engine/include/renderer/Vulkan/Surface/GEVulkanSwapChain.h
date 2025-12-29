#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/Vulkan/core/GEVulkanPhysicalDevice.h"
#include "renderer/Vulkan/core/GEVulkanLogicalDevice.h"
#include "renderer/Vulkan/Resources/GEVulkanImageView.h"
#include "renderer/Vulkan/Render/GEVulkanRenderPass.h"
#include "renderer/Vulkan/Resources/GEVulkanFramebuffer.h"

class GEVulkanSwapChain {
private:
	int m_width = 0;
	int m_height = 0;


	VkSwapchainKHR m_vkSwapChain = VK_NULL_HANDLE;
	VkSurfaceFormatKHR m_vkSurfaceFormat {};
	VkPresentModeKHR m_vkPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkExtent2D m_vkExtent{};

	GEVulkanPhysicalDevice* m_physicalDevice;
	GEVulkanSurfaceView* m_surfaceView;
	GEVulkanLogicalDevice* m_logicalDevice;
	GEVulkanRenderPass* m_renderPass = nullptr;

	std::vector<VkImage> m_vkSwapChainImages;
	std::vector<GEVulkanImageView> m_vkSwapChainImageViews;
	std::vector<GEVulkanFramebuffer> m_vkFramebuffers;

	void ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateImageViews(uint32_t imageCount);
public:
	GEVulkanSwapChain(GEVulkanPhysicalDevice* physicalDevice, GEVulkanLogicalDevice* logicalDevice, GEVulkanSurfaceView *surface,uint32_t width, uint32_t height) : 
		m_physicalDevice(physicalDevice),
		m_logicalDevice(logicalDevice),
		m_surfaceView(surface), 
		m_width(width), 
		m_height(height) {};

	~GEVulkanSwapChain() = default;

	VkFormat GetSwapChainImageFormat() const { return m_vkSurfaceFormat.format; }
	void CreateSwapChain();
	void CreateFramebuffers();

	void Cleanup();

	void SetRenderPass(GEVulkanRenderPass* renderPass) { m_renderPass = renderPass; }

	VkFramebuffer GetFramebuffer(uint32_t index) { return m_vkFramebuffers[index].GetVkFramebuffer(); }
	VkExtent2D GetSwapChainExtent() const { return m_vkExtent; }
};
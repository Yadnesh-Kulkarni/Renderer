#include <iostream>
#include <algorithm>
#include "renderer/Vulkan/GEVulkanSwapChain.h"


void GEVulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			m_vkSurfaceFormat = availableFormat;
			return;
		}
	}
	m_vkSurfaceFormat = availableFormats[0];
}

void GEVulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	m_vkPresentMode =  VK_PRESENT_MODE_FIFO_KHR;
}

void GEVulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{

	if (capabilities.currentExtent.height != UINT32_MAX)
	{
		m_vkExtent = capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		m_vkExtent = actualExtent;
	}
}

void GEVulkanSwapChain::CreateSwapChain()
{
	SwapchainSupportDetails swapchainSupport = m_physicalDevice->GetSwapchainSupportDetails();
	QueueFamilyIndices indices = m_physicalDevice->GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	ChooseSwapSurfaceFormat(swapchainSupport.formats);
	ChooseSwapPresentMode(swapchainSupport.presentModes);
	ChooseSwapExtent(swapchainSupport.surfaceCapabilities);

	uint32_t imageCount = swapchainSupport.surfaceCapabilities.minImageCount + 1;

	if (swapchainSupport.surfaceCapabilities.maxImageCount > 0 && imageCount > swapchainSupport.surfaceCapabilities.maxImageCount)
	{
		imageCount = swapchainSupport.surfaceCapabilities.maxImageCount;
	}



	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surfaceView->getSurfaceView();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_vkSurfaceFormat.format;
	createInfo.imageColorSpace = m_vkSurfaceFormat.colorSpace;
	createInfo.imageExtent = m_vkExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (indices.graphicsFamily != indices.presentFamily) 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} 
	else 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapchainSupport.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_vkPresentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_logicalDevice->getVkDevice(), &createInfo, nullptr, &m_vkSwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_logicalDevice->getVkDevice(), m_vkSwapChain, &imageCount, nullptr);
	m_vkSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_logicalDevice->getVkDevice(), m_vkSwapChain, &imageCount, m_vkSwapChainImages.data());
}

void GEVulkanSwapChain::Cleanup()
{
	if (m_vkSwapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_logicalDevice->getVkDevice(), m_vkSwapChain, nullptr);
		m_vkSwapChain = VK_NULL_HANDLE;
	}
}

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "renderer/Vulkan/core/GEVulkanPhysicalDevice.h"

int GEVulkanPhysicalDevice::rateDeviceSuitablity(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

QueueFamilyIndices GEVulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) 
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i , m_vkSurfaceView.getSurfaceView(), &presentSupport);
		if (presentSupport) {
			indices.presentFamily = i;
		}

		i++;
	}

    return indices;
}

SwapchainSupportDetails GEVulkanPhysicalDevice::querySwapchainSupport(VkPhysicalDevice device)
{
	SwapchainSupportDetails details;

	// Device Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_vkSurfaceView.getSurfaceView(), &details.surfaceCapabilities);
	
	// Device Surface Formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurfaceView.getSurfaceView(), &formatCount, nullptr);


	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vkSurfaceView.getSurfaceView(), &formatCount, details.formats.data());
	}

	// Device Present Modes
	uint32_t presentModeCount;

	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurfaceView.getSurfaceView(), &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vkSurfaceView.getSurfaceView(), &presentModeCount, details.presentModes.data());
	}


	return details;
}

bool GEVulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, GEPhysicalDeviceDetails& details)
{
	details.queueFamilyIndices = findQueueFamilies(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if(extensionsSupported)
	{
		details.swapchainSupportDetails = querySwapchainSupport(device);
		swapChainAdequate = !details.swapchainSupportDetails.formats.empty() && !details.swapchainSupportDetails.presentModes.empty();
	}

    return details.queueFamilyIndices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool GEVulkanPhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void GEVulkanPhysicalDevice::pickPhysicalDevice(VkInstance instance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	
	std::multimap<int, std::pair<VkPhysicalDevice, GEPhysicalDeviceDetails>> candidates;
	for (const auto& device : devices) {
		GEPhysicalDeviceDetails deviceDetails;
		if(isDeviceSuitable(device, deviceDetails))
		{
			int score = rateDeviceSuitablity(device);
			candidates.insert(std::make_pair(score, std::make_pair(device, deviceDetails)));
		}
	}

	 if (candidates.rbegin()->first > 0) 
	 {
		m_vkPhysicalDevice = candidates.rbegin()->second.first;
		m_deviceDetails = candidates.rbegin()->second.second;
     } 
	 else 
	 {
        throw std::runtime_error("failed to find a suitable GPU!");
     }
}

void GEVulkanPhysicalDevice::RefreshSwapchainSupportDetails()
{
	m_deviceDetails.swapchainSupportDetails = querySwapchainSupport(m_vkPhysicalDevice);
}

VkFormat GEVulkanPhysicalDevice::FindSupportedFormat(
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_vkPhysicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat GEVulkanPhysicalDevice::ResolveDepthBufferFormat()
{
	return FindSupportedFormat(
		{ DEPTH_BUFFER_PREFERRED_FORMAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void GEVulkanPhysicalDevice::Cleanup()
{
	m_vkPhysicalDevice = VK_NULL_HANDLE;
}



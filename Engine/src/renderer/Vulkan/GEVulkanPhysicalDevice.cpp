#include <iostream>
#include <vector>
#include <map>
#include "renderer/Vulkan/GEVulkanPhysicalDevice.h"

int GEVulkanPhysicalDevice::rateDeviceSuitablity(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

QueueFamilyIndices GEVulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device)
{
	 QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
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

		i++;
	}

    return indices;
}

bool GEVulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.graphicsFamily.has_value();
}

void GEVulkanPhysicalDevice::pickPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices) {
		if(isDeviceSuitable(device))
		{
			int score = rateDeviceSuitablity(device);
			candidates.insert(std::make_pair(score, device));
		}
	}

	 if (candidates.rbegin()->first > 0) {
        m_vkPhysicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}



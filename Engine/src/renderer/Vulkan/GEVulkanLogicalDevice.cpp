#include <iostream>
#include <vector>
#include <set>
#include "renderer/Vulkan/GEVulkanLogicalDevice.h"
#include "renderer/Vulkan/common/GEVulkancommon.h"

VkQueue GEVulkanLogicalDevice::CreateVkQueue(uint32_t queueFamilyIndex)
{
	return VkQueue();
}

void GEVulkanLogicalDevice::createLogicalDevice()
{
	QueueFamilyIndices indices = m_vkPhysicalDevice->GetQueueFamilyIndices();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) 
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(m_vkPhysicalDevice->GetPhysicalDevice(), &createInfo, nullptr, &m_vkDevice) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_vkDevice, indices.graphicsFamily.value(), 0, &m_vkGraphicsQueue);
	vkGetDeviceQueue(m_vkDevice, indices.presentFamily.value(), 0, &m_vkPresentQueue);
}

void GEVulkanLogicalDevice::cleanUp()
{
	if (m_vkDevice != VK_NULL_HANDLE) 
	{
		vkDestroyDevice(m_vkDevice, nullptr);
		m_vkDevice = VK_NULL_HANDLE;
	}
}

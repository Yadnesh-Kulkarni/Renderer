#include <iostream>
#include <vector>
#include "renderer/Vulkan/GEVulkanLogicalDevice.h"
#include "renderer/Vulkan/common/GEVulkancommon.h"

void GEVulkanLogicalDevice::createLogicalDevice()
{
	QueueFamilyIndices indices = m_vkPhysicalDevice.findQueueFamilies(m_vkPhysicalDevice.GetPhysicalDevice());

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(m_vkPhysicalDevice.GetPhysicalDevice(), &createInfo, nullptr, &m_vkDevice) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_vkDevice, indices.graphicsFamily.value(), 0, &m_vkGraphicsQueue);
}

void GEVulkanLogicalDevice::cleanUp()
{
	if (m_vkDevice != VK_NULL_HANDLE) 
	{
		vkDestroyDevice(m_vkDevice, nullptr);
		m_vkDevice = VK_NULL_HANDLE;
	}
}

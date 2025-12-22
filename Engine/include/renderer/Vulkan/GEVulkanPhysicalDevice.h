#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Vulkan/GEVulkanSurfaceView.h"
#include "renderer/Vulkan/common/GEVulkancommon.h"

class GEVulkanPhysicalDevice {

private:
	GEVulkanSurfaceView m_vkSurfaceView;
	VkPhysicalDevice m_vkPhysicalDevice;
	QueueFamilyIndices m_queueFamilyIndices {};

	int rateDeviceSuitablity(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices& indices);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
public:
	
    GEVulkanPhysicalDevice(const GEVulkanPhysicalDevice& other)
        : m_vkSurfaceView(other.m_vkSurfaceView),
          m_vkPhysicalDevice(other.m_vkPhysicalDevice),
          m_queueFamilyIndices(other.m_queueFamilyIndices) {}

	GEVulkanPhysicalDevice(const GEVulkanSurfaceView& view) : m_vkSurfaceView(view) {};
	~GEVulkanPhysicalDevice() = default;

	void pickPhysicalDevice(VkInstance instance);
	const VkPhysicalDevice GetPhysicalDevice() { return m_vkPhysicalDevice; }

	const QueueFamilyIndices GetQueueFamilyIndices() { return m_queueFamilyIndices; }
	void Cleanup();
};
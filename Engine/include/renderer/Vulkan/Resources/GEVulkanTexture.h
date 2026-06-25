#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include "renderer/Vulkan/Resources/GEVulkanImage.h"
#include "renderer/Vulkan/Resources/GEVulkanStagingDevice.h"

class GEVulkanTexture {
public:
	static VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device);
	static void DestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout);

	void LoadFromFile(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		GEVulkanStagingDevice& staging,
		VkDescriptorSetLayout descriptorSetLayout,
		const std::string& path);

	void Cleanup(VkDevice device);

	VkDescriptorSet GetDescriptorSet() const { return m_descriptorSet; }

private:
	GEVulkanImage m_image;
	VkSampler m_sampler = VK_NULL_HANDLE;
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
};

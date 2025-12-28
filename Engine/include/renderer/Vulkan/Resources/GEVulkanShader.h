#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class GEVulkanShader {
private:
	VkShaderModule m_shaderModule = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	std::vector<char> readFile(const std::string& filename);

public:
	GEVulkanShader(VkDevice device) : m_device(device) {}
	~GEVulkanShader() = default;

	VkShaderModule GetShaderModule() const { return m_shaderModule; }

	VkShaderModule CreateShaderModule(const std::string& fileName);
	void Cleanup();
};
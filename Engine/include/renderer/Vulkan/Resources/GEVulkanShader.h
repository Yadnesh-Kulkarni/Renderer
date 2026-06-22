#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

class GEVulkanShader {
private:
	VkShaderModule m_shaderModule = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;

	std::vector<char> readFile(const std::string& filename);
	std::string readTextFile(const std::string& path);
	shaderc_shader_kind VkStageToShadercKind(VkShaderStageFlagBits stage);

public:
	GEVulkanShader(VkDevice device) : m_device(device) {}
	~GEVulkanShader() = default;

	VkShaderModule GetShaderModule() const { return m_shaderModule; }

	VkShaderModule CreateShaderModule(const std::string& fileName);
	VkShaderModule CreateShaderModuleFromGLSL(const std::string& sourcePath, VkShaderStageFlagBits stage);
	void Cleanup();
};

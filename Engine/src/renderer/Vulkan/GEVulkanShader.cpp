#include "renderer/Vulkan/GEVulkanShader.h"

std::vector<char> GEVulkanShader::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
	
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule GEVulkanShader::CreateShaderModule(const std::string& filename)
{
	std::vector<char> code = readFile(filename);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

    return m_shaderModule;
}

void GEVulkanShader::Cleanup()
{
    if(m_shaderModule)
    {
        vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
    }
}

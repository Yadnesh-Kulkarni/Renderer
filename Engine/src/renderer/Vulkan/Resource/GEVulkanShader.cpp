#include <stdexcept>
#include <sstream>
#include "renderer/Vulkan/Resources/GEVulkanShader.h"
#include "utils/FileUtils.h"

std::vector<char> GEVulkanShader::readFile(const std::string& filename)
{
	std::ifstream file(FileUtils::ResolvePath(filename), std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("failed to open file: " + FileUtils::ResolvePath(filename));

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

std::string GEVulkanShader::readTextFile(const std::string& path)
{
	std::string resolved = FileUtils::ResolvePath(path);
	std::ifstream file(resolved);

	if (!file.is_open())
		throw std::runtime_error("failed to open shader source: " + resolved);

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

shaderc_shader_kind GEVulkanShader::VkStageToShadercKind(VkShaderStageFlagBits stage)
{
	switch (stage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:                  return shaderc_glsl_vertex_shader;
		case VK_SHADER_STAGE_FRAGMENT_BIT:                return shaderc_glsl_fragment_shader;
		case VK_SHADER_STAGE_GEOMETRY_BIT:                return shaderc_glsl_geometry_shader;
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:    return shaderc_glsl_tess_control_shader;
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return shaderc_glsl_tess_evaluation_shader;
		case VK_SHADER_STAGE_COMPUTE_BIT:                 return shaderc_glsl_compute_shader;
		default:
			throw std::runtime_error("unsupported shader stage");
	}
}

VkShaderModule GEVulkanShader::CreateShaderModule(const std::string& filename)
{
	std::vector<char> code = readFile(filename);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");

	return m_shaderModule;
}

VkShaderModule GEVulkanShader::CreateShaderModuleFromGLSL(
	const std::string& sourcePath,
	VkShaderStageFlagBits stage)
{
	std::string source = readTextFile(sourcePath);

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);

	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
		source,
		VkStageToShadercKind(stage),
		sourcePath.c_str(),
		options);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		throw std::runtime_error("Shader compile error in '" + sourcePath + "':\n" + result.GetErrorMessage());

	std::vector<uint32_t> spirv(result.cbegin(), result.cend());

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = spirv.size() * sizeof(uint32_t);
	createInfo.pCode    = spirv.data();

 	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module from GLSL!");

	return m_shaderModule;
}

void GEVulkanShader::Cleanup()
{
	if (m_shaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
		m_shaderModule = VK_NULL_HANDLE;
	}
}

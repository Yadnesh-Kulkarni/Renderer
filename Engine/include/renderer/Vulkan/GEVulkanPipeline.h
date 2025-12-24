#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include "renderer/Vulkan/GEVulkanShader.h"
#include "renderer/Vulkan/GEVulkanFrameContext.h"

class GEVulkanPipeline {

private:
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

	std::unique_ptr<GEVulkanShader> m_vertexShader;
	std::unique_ptr<GEVulkanShader> m_fragmentShader;
	GEVulkanFrameContext m_frameContext;

public:
	GEVulkanPipeline(VkDevice& device, GEVulkanFrameContext frameContext) : m_device(device), m_frameContext(frameContext) {
		m_vertexShader = std::make_unique<GEVulkanShader>(m_device);
		m_fragmentShader = std::make_unique<GEVulkanShader>(m_device);
	};

	~GEVulkanPipeline() = default;

	void CreatePipeline();
	void Cleanup();
};
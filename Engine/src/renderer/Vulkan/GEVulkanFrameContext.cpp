#include "renderer/Vulkan/GEVulkanFrameContext.h"

void GEVulkanFrameContext::CreateFrameContext()
{
	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	m_viewport.width = static_cast<float>(m_width);
	m_viewport.height = static_cast<float>(m_height);
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;


	m_scissor.offset = { 0, 0 };
	m_scissor.extent = { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) };
}

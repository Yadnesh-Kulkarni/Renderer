#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "renderer/Renderer.h"
#include "renderer/Vulkan/core/GEVulkanValidationLayer.h"
#include "renderer/Vulkan/core/GEVulkanPhysicalDevice.h"
#include "renderer/Vulkan/core/GEVulkanLogicalDevice.h"
#include "renderer/Vulkan/Surface/GEVulkanSurfaceView.h"
#include "renderer/Vulkan/Surface/GEVulkanSwapChain.h"
#include "renderer/Vulkan/Render/GEVulkanFrameContext.h"
#include "renderer/Vulkan/Render/GEVulkanRenderPass.h"
#include "renderer/Vulkan/Render/GEVulkanPipeline.h"
#include "renderer/Vulkan/core/GEVulkanCommandPool.h"
#include "renderer/Vulkan/Resources/GEVulkanBuffer.h"
#include "renderer/Vulkan/common/GEVulkancommon.h"
#include "utils/MeshLoader.h"

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include "math/GLM/vec4.hpp"
//#include "math/GLM/mat4x4.hpp"

class VulkanRenderer : public Renderer{
private:
	uint32_t currentImageIndex = 0;
	std::vector<VkFence> imagesInFlight;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	bool frameReady = false;
	float m_rotationAngle = 0.0f;
	VkFormat m_depthFormat = VK_FORMAT_UNDEFINED;

	VkInstance vkInstance;
   
    GEVulkanValidationLayer vkValidationLayer;
    GERequiredExtensions requiredExtensions;
	std::unique_ptr<GEVulkanPhysicalDevice> vkPhysicalDevice;
	std::unique_ptr<GEVulkanLogicalDevice> vkLogicalDevice;
	std::unique_ptr<GEVulkanSurfaceView> vkSurfaceView;
	std::unique_ptr<GEVulkanSwapChain> vkSwapChain;
	std::unique_ptr<GEVulkanFrameContext> frameContext;
	std::unique_ptr<GEVulkanRenderPass> vkRenderPass;
	std::unique_ptr<GEVulkanPipeline> vkPipeline;
	std::unique_ptr<GEVulkanPipeline> vkPipelineWireframe;
	std::unique_ptr<GEVulkanCommandPool> vkCommandPool;
	MeshLoader m_meshLoader;
	GEVulkanBuffer m_vertexBuffer;
	GEVulkanBuffer m_indexBuffer;
	uint32_t m_indexCount = 0;

    void CreateInstance();
	void RecordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void CreateRenderFinishedSemaphores(uint32_t imageCount);
	void DestroyRenderFinishedSemaphores();
	void RecreateSwapChain();

protected:
    virtual void BeginFrame();
    virtual void EndFrame();
	virtual void DrawFrame();
	virtual void WaitIdle();

    virtual void SwapBuffers();

    void PrintInstanceExtensions();
public:
    VulkanRenderer(WindowCreator* window);
    ~VulkanRenderer();

    virtual void Initialize();
	virtual void Cleanup();

	virtual void SetRequiredExtensions();

	void SetRotationAngle(float angleDegrees) { m_rotationAngle = angleDegrees; }
};
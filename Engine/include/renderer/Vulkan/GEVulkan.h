#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "renderer/Renderer.h"
#include "renderer/Vulkan/GEVulkanValidationLayer.h"
#include "renderer/Vulkan/GEVulkanPhysicalDevice.h"
#include "renderer/Vulkan/GEVulkanLogicalDevice.h"
#include "renderer/Vulkan/GEVulkanSurfaceView.h"
#include "renderer/Vulkan/GEVulkanSwapChain.h"

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include "math/GLM/vec4.hpp"
//#include "math/GLM/mat4x4.hpp"

class VulkanRenderer : public Renderer{
private:
	VkInstance vkInstance;
   
    GEVulkanValidationLayer vkValidationLayer;
    GERequiredExtensions requiredExtensions;
	std::unique_ptr<GEVulkanPhysicalDevice> vkPhysicalDevice;
	std::unique_ptr<GEVulkanLogicalDevice> vkLogicalDevice;
	std::unique_ptr<GEVulkanSurfaceView> vkSurfaceView;
	std::unique_ptr<GEVulkanSwapChain> vkSwapChain;
    void CreateInstance();
protected:
    virtual void BeginFrame();
    virtual void EndFrame();

    virtual void SwapBuffers();

    void PrintInstanceExtensions();
public:
    VulkanRenderer(WindowCreator* window);
    ~VulkanRenderer();

    virtual void Initialize();
	virtual void Cleanup();

	virtual void SetRequiredExtensions(GERequiredExtensions* pRequiredExtensions);
};
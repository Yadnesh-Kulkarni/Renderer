#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Renderer.h"
#include "renderer/Vulkan/GEVulkanValidationLayer.h"

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include "math/GLM/vec4.hpp"
//#include "math/GLM/mat4x4.hpp"

class VulkanRenderer : public Renderer{
private:
	VkInstance vkInstance;
    GEVulkanValidationLayer vkValidationLayer;
    GERequiredExtensions requiredExtensions;

    void CreateInstance();
protected:
    virtual void BeginFrame();
    virtual void EndFrame();

    virtual void SwapBuffers();

    void PrintInstanceExtensions();
public:
    VulkanRenderer();
    ~VulkanRenderer();

    virtual void Initialize();
	virtual void Cleanup();

	virtual void SetRequiredExtensions(GERequiredExtensions* pRequiredExtensions);
};
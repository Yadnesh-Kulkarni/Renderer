#pragma once
// Renderer header
#include <vector>
#include "common/common.h"
#include "renderer/Vulkan/GEVulkanValidationLayer.h"


class Renderer {
public:
    Renderer()  {}
    ~Renderer() = default;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void SwapBuffers() = 0;
    virtual void Initialize() = 0;
    virtual void Cleanup() = 0;

    virtual void SetRequiredExtensions(GERequiredExtensions* pRequiredExtensions) {};
};
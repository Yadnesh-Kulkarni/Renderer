#pragma once
// Renderer header
#include <vector>
#include "common/common.h"
#include "renderer/Vulkan/GEVulkanValidationLayer.h"
#include "platform/Window.h"

class Renderer {
protected:
    WindowCreator* m_window;
public:
    Renderer(WindowCreator* window)  { m_window = window; }
    ~Renderer() = default;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void SwapBuffers() = 0;
    virtual void Initialize() = 0;
    virtual void Cleanup() = 0;

    virtual void SetRequiredExtensions(GERequiredExtensions* pRequiredExtensions) {};
};
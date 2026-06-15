#include "core/Engine.h"
#include "renderer/Vulkan/core/GEVulkan.h"

EngineCore::EngineCore()
{
    window = std::make_unique<WindowCreator>();
    renderer = std::make_unique<VulkanRenderer>(window.get());
}

EngineCore::~EngineCore()
{

}

void EngineCore::Run()
{
    int width = 1280;
	int height = 720;
    window->createWindow(width, height, "Vulkan window");

    GERequiredExtensions extensionInfo;
    window->getRequiredExtensions(&extensionInfo);
	renderer->SetRequiredExtensions(&extensionInfo);
    renderer->Initialize();

    while(!window->shouldClose())
    {
        window->pollEvents();
    }

    renderer->Cleanup();
}
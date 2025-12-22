#include "core/Engine.h"
#include "renderer/Vulkan/GEVulkan.h"

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
    window->createWindow(800, 600, "Vulkan window");

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
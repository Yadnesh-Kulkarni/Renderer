#include "core/Engine.h"

EngineCore::EngineCore()
{
	window = new WindowCreator();
	renderer = new Renderer();
}

EngineCore::~EngineCore()
{
	if(window)
    {
        window->destroyWindow();
        delete window;
    }
}

void EngineCore::Run()
{
    window->createWindow(800, 600, "Vulkan window");

    GERequiredExtensions extensionInfo;
    window->getRequiredExtensions(&extensionInfo);
    renderer->Initialize(extensionInfo);

    while(!window->shouldClose())
    {
        window->pollEvents();
    }
}
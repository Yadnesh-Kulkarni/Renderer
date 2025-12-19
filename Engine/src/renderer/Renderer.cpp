// Renderer implementation
#include <iostream>
#include "renderer/Renderer.h"

Renderer::Renderer()
{
    windowCreator = new WindowCreator();
    windowCreator->createWindow(800, 600, "Vulkan window");
}

Renderer::~Renderer()
{
    if(windowCreator)
    {
        windowCreator->destroyWindow();
        delete windowCreator;
    }
}

void Renderer::Initialize()
{

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported\n";

}

void Renderer::GameLoop()
{
    windowCreator->pollEvents();
}

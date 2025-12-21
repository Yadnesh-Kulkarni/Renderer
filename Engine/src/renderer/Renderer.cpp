// Renderer implementation
#include <iostream>
#include "renderer/Renderer.h"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
   
}

void Renderer::Initialize(GERequiredExtensions requiredExtensions)
{
    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Engine Instance";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0 , 0);
    appInfo.pEngineName = "Game";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0 ,0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.count;
    createInfo.ppEnabledExtensionNames = requiredExtensions.extensions;
    createInfo.enabledLayerCount = 0;

    if(vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
    {
        std::runtime_error("Unable to create Vulkan Instance");
    }
}

void Renderer::PrintInstanceExtensions()
{
	uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported\n";
    instanceExtensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

    for(const VkExtensionProperties &e : instanceExtensions)
    {
        std::cout << e.extensionName << std::endl;
    }

    std::cout << std::endl << std::endl;
}

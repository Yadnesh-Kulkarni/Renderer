#include <iostream>
#include <vector>
#include "renderer/Vulkan/GEVulkan.h"


void VulkanRenderer::CreateInstance()
{
    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Engine Instance";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0 , 0);
    appInfo.pEngineName = "Game";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0 ,0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    // TODO : Refactor extension concat to a better solution
    //        Preferably by creating a new class called ExtensionManager
    //        Need to figure out how to manage data flow between glfwGetRequiredExtensions and vulkanExtensions that we need

    std::vector<const char *> Extensions(requiredExtensions.extensions, requiredExtensions.extensions + requiredExtensions.count);
    if(vkValidationLayer.IsValidationLayerEnabled())
    {    
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = Extensions.size();
    createInfo.ppEnabledExtensionNames = Extensions.data();

    if(vkValidationLayer.IsValidationLayerEnabled() && !vkValidationLayer.CheckForValidationSupport())
    {
        createInfo.enabledLayerCount = vkValidationLayer.GetValidationLayersCount();
        createInfo.ppEnabledLayerNames = vkValidationLayer.GetValidationLayers();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
    {
        std::runtime_error("Unable to create Vulkan Instance");
    }

    vkValidationLayer.SetupDebugMessenger(vkInstance);
}

VulkanRenderer::VulkanRenderer()
{
	vkPhysicalDevice = std::make_unique<GEVulkanPhysicalDevice>();
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Initialize()
{
	CreateInstance();
	vkPhysicalDevice->pickPhysicalDevice(vkInstance, nullptr);
	vkLogicalDevice = std::make_unique<GEVulkanLogicalDevice>(*vkPhysicalDevice);
	vkLogicalDevice->createLogicalDevice();
}

void VulkanRenderer::Cleanup()
{
    if(vkPhysicalDevice)
    {
		vkPhysicalDevice->Cleanup();
		vkPhysicalDevice.reset();
	}

    if(vkLogicalDevice)
    {
		vkLogicalDevice->cleanUp();
		vkLogicalDevice.reset();
	}

    if (vkValidationLayer.IsValidationLayerEnabled()) {
       vkValidationLayer.DestroyDebugUtilsMessengerEXT(vkInstance, nullptr);
    }

    if(vkInstance)
    {
        vkDestroyInstance(vkInstance, nullptr);
    }
}

void VulkanRenderer::BeginFrame()
{
}

void VulkanRenderer::EndFrame()
{
}

void VulkanRenderer::SwapBuffers()
{
}

void VulkanRenderer::PrintInstanceExtensions()
{
    std::vector<VkExtensionProperties> instanceExtensions;
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

void VulkanRenderer::SetRequiredExtensions(GERequiredExtensions* pRequiredExtensions) 
{ 
    requiredExtensions = *pRequiredExtensions; 
}


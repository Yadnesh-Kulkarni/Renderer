#include "renderer/Vulkan/core/GEVulkanValidationLayer.h"
#include <iostream>

GEVulkanValidationLayer::GEVulkanValidationLayer()
{
#ifdef ENGINE_DEBUG
	bEnableValidationLayer = true;
#else
	bEnableValidationLayer = false;
#endif
}

GEVulkanValidationLayer::~GEVulkanValidationLayer()
{
   
}

bool GEVulkanValidationLayer::CheckForValidationSupport()
{
    if(bEnableValidationLayer)
    {
	    uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) 
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) 
            {
                if (strcmp(layerName, layerProperties.layerName) == 0) 
                {
                    layerFound = true;
                    break;
                }
            }

            if(!layerFound)
            {
                std::cout << "Could not enable Vulkan Validation Layer" << std::endl;
                return false;
            }
        }
    }

    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GEVulkanValidationLayer::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void GEVulkanValidationLayer::SetupDebugMessenger(VkInstance instance)
{
    if(!bEnableValidationLayer)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = GEVulkanValidationLayer::debugCallback;
    createInfo.pUserData = nullptr; // Optional
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult GEVulkanValidationLayer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void GEVulkanValidationLayer::DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator) 
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
    

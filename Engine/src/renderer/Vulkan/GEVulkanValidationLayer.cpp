#include "renderer/Vulkan/GEVulkanValidationLayer.h"
#include <iostream>

GEVulkanValidationLayer::GEVulkanValidationLayer()
{
#ifdef ENGINE_DEBUG
	bEnableValidationLayer = true;
#else
	bEnableValidationLayer = false;
#endif
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

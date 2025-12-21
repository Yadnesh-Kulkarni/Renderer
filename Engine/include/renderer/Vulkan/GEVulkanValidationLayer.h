#pragma once
#include <vector>
#include <vulkan/vulkan.h>

struct GEValidationLayerInfo{
};

class GEVulkanValidationLayer {
private:
	bool bEnableValidationLayer;
	VkDebugUtilsMessengerEXT debugMessenger;
	const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
public:
	GEVulkanValidationLayer();
	~GEVulkanValidationLayer();

	bool const IsValidationLayerEnabled(){ return bEnableValidationLayer; }
	uint32_t GetValidationLayersCount() { return static_cast<uint32_t>(validationLayers.size()); }
	const char* const* GetValidationLayers() { return validationLayers.data(); } 

	bool CheckForValidationSupport();

	// Debug Callback
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														void* pUserData);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger); 
	void SetupDebugMessenger(VkInstance instance);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator);
};
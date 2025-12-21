#pragma once
#include <vector>
#include <vulkan/vulkan.h>

struct GEValidationLayerInfo{
};

class GEVulkanValidationLayer {
private:
	bool bEnableValidationLayer;
	const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
public:
	GEVulkanValidationLayer();

	bool const IsValidationLayerEnabled(){ return bEnableValidationLayer; }
	uint32_t GetValidationLayersCount() { return static_cast<uint32_t>(validationLayers.size()); }
	const char* const* GetValidationLayers() { return validationLayers.data(); } 

	bool CheckForValidationSupport();
};